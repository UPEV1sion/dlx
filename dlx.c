#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define shift_args(argc, argv) ((argc)--, *(argv)++)

typedef struct Node Node;
typedef struct Column Column;

struct Node {
    Node *l, *r, *d, *u;
    Column *c;
};

struct Column {
    Node header;
    char *name;
    size_t count;
};

Column* parse_matrix(const char *path, size_t *ncols, size_t *nrows, size_t *npieces)
{
    FILE *f;
    assert((f = fopen(path, "r")));

    fscanf(f, "%zu", ncols);
    fscanf(f, "%zu", nrows);
    fscanf(f, "%zu", npieces);

    Column *columns = malloc(*ncols * sizeof(Column));
    assert(columns);
    
    for(size_t i = 0; i < *ncols; ++i)
    {
        columns[i].name = NULL;
        columns[i].count = 0;
        columns[i].header.c = &columns[i];
        columns[i].header.d = &columns[i].header;
        columns[i].header.u = &columns[i].header;
    } 
    
    Node root;
    root.r = root.l = &root;

    for(size_t i = 0; i < *ncols; ++i)
    {
        Node *col_node = &columns[i].header;

        col_node->r = root.r;
        col_node->l = &root;
        root.r->l = col_node;
        root.r = col_node;
    }
    while (1)
    {
        size_t col;

        if (fscanf(f, "%zu", &col) != 1)
            break;

        Node *first = NULL;
        Node *prev = NULL;

        do
        {
            Node *node = malloc(sizeof(Node));

            node->c = &columns[col];

            node->d = &columns[col].header;
            node->u = columns[col].header.u;
            columns[col].header.u->d = node;
            columns[col].header.u = node;
            columns[col].count++;

            if (!first)
            {
                first = node;
                node->l = node->r = node;
            }
            else
            {
                node->l = prev;
                node->r = first;
                prev->r = node;
                first->l = node;
            }

            prev = node;

            const int c = fgetc(f);
            if (c == '\n' || c == EOF)
                break;

            ungetc(c, f);

        } while (fscanf(f, "%zu", &col) == 1);
    }

    fclose(f);

    return columns;
}

int main(int argc, char **argv)
{
    const char *program = shift_args(argc, argv);
    if(argc <= 0)
    {
        fprintf(stderr, "USAGE: %s <matrix>\n", program);
        return 1;
    }

    const char *path = shift_args(argc, argv);
    size_t ncols, nrows, npieces;
    Column *column = parse_matrix(path, &ncols, &nrows, &npieces);


    return 0;
}
