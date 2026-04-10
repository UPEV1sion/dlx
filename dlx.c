#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_COLS 1024
#define MAX_NODES (MAX_COLS * 1024)

typedef struct Column Column;
typedef struct Node Node;

struct Node {
    Node *l, *r, *u, *d;
    Column *c;
};

struct Column {
    Node header;
    size_t s;
    size_t n;
};

typedef struct {
    Node *root;
    Column *columns;    
} DLX;

Node* new_node(Column *c);

void columns_add_node(Column *c, Node *n)
{
    Node *header = &c->header;
    n->u = header->u;
    n->d = header;
    header->u->d = n;
    header->u = n;
    n->c = c;
    c->s++;
}

void link_row(Node **row_nodes, const size_t count)
{
    for(size_t i = 0; i < count; ++i)
    {
        row_nodes[i]->r = row_nodes[(i + 1) % count];
        row_nodes[i]->l = row_nodes[(i + count - 1) % count];
    }
}

void cover_column(Column *c)
{
    c->header.r->l = c->header.l;
    c->header.l->r = c->header.r;
    const Node *start = &c->header;
    for(Node *i = start->d; i != start; i = i->d)
    {
        for(Node *j = i->r; j != i; j = j->r)
        {
            j->d->u = j->u;
            j->u->d = j->d;
            j->c->s--;
        }
    }
}

void uncover_column(Column *c)
{
    const Node *start = &c->header;
    for(Node *i = start->u; i != start; i = i->u)
    {
        for(Node *j = i->l; j != i; j = j->l)
        {
            j->c->s++;
            j->d->u = j;
            j->u->d = j;
        }
    }
    
    c->header.r->l = &c->header;
    c->header.l->r = &c->header;
}

DLX parse_dlx_from_file(size_t *nrows, size_t *ncols, size_t *nmand)
{
    DLX dlx;
    assert(scanf("%zu", nrows) == 1);
    assert(scanf("%zu", ncols) == 1);
    assert(scanf("%zu", nmand) == 1);
    dlx.columns = malloc(*ncols * sizeof(Column));
    assert(dlx.columns);

    dlx.root = new_node(NULL);

    for(size_t i = 0; i < *nmand; ++i)
    {
        dlx.columns[i].s = 0;
        dlx.columns[i].n = i;
        dlx.columns[i].header.c = &dlx.columns[i];
        dlx.columns[i].header.u = &dlx.columns[i].header;
        dlx.columns[i].header.d = &dlx.columns[i].header;
        dlx.columns[i].header.l = dlx.root;
        dlx.columns[i].header.r = dlx.root->r;

        dlx.root->r->l = &dlx.columns[i].header;
        dlx.root->r = &dlx.columns[i].header;
    }

    for(size_t i = *nmand; i < *ncols; ++i)
    {
        dlx.columns[i].s = 0;
        dlx.columns[i].n = i;
        dlx.columns[i].header.c = &dlx.columns[i];
        dlx.columns[i].header.u = &dlx.columns[i].header;
        dlx.columns[i].header.d = &dlx.columns[i].header;
        dlx.columns[i].header.l = &dlx.columns[i].header;
        dlx.columns[i].header.r = &dlx.columns[i].header;
    }

    char line[MAX_COLS * 2];
    while(fgets(line, sizeof line, stdin))
    {
        size_t count = 0;
        size_t positions[MAX_COLS];

        size_t idx;
        char *ptr = line;
        while(sscanf(ptr, "%zu", &idx) == 1)
        {
            positions[count++] = idx;

            
            while(*ptr && *ptr != ' ') ptr++;
            while(*ptr == ' ') ptr++;
        }
        
        if(count == 0) continue;

        Node *row_nodes[MAX_COLS];
        for(size_t i = 0; i < count; ++i)
        {
            Column *col = dlx.columns + positions[i];
            Node *n = new_node(col);
            columns_add_node(col, n);
            row_nodes[i] = n;
        }

        link_row(row_nodes, count);
    }

    return dlx;
}

Node* choose_column(Node *root)
{
    Node *best = root->r;

    for(Node *c = root->r; c != root; c = c->r)
    {
        if(c->c->s < best->c->s) best = c;
    }

    return best;
}

void print_solution(Node **o, const size_t k)
{
    for(size_t i = 0; i < k; ++i)
    {
        Node *row = o[i];

        Node *j = row;
        do
        {
            printf("%zu ", j->c->n);
            j = j->r;
        } while(j != row);

        printf("\n");
    }

    printf("\n");
}

void search(DLX dlx, Node **o, size_t k)
{
    if(dlx.root == dlx.root->r) 
    {
        print_solution(o, k);
        return;
    }
    
    Node *c_node = choose_column(dlx.root);
    Column *c = c_node->c;
    cover_column(c);

    for(Node *r = c->header.d; r != &c->header; r = r->d)
    {
        o[k] = r;

        for(Node *j = r->r; j != r; j = j->r)
        {
            cover_column(j->c);
        }

        search(dlx, o, k + 1);

        for(Node *j = r->l; j != r; j = j->l)
        {
            uncover_column(j->c);
        }
    }

    uncover_column(c);
}

int main(void)
{
    size_t nrows, ncols, nmand;
    DLX dlx = parse_dlx_from_file(&nrows, &ncols, &nmand);
    Node **o = malloc(sizeof(Node*) * ncols);
    assert(o);
    search(dlx, o, 0);
    free(o);
    free(dlx.columns);

    return 0;
}

static Node node_pool[MAX_NODES];
static size_t node_ptr = 0;

Node* new_node(Column *c)
{
    assert(node_ptr <= MAX_NODES);
    Node *n = &node_pool[node_ptr++];
    n->l = n->r = n->u = n->d = n;
    n->c = c;
    return n;
}
