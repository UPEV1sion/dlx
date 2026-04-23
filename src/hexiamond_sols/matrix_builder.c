#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdbool.h>

#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))
#define NUM_PIECES (ARRAY_LEN(pieces))

#define WIDTH  6
#define HEIGHT 15

#define NROWS 0
#define NCOLS 0
#define NMAND 0

typedef struct {
    int x, y, z;
} Cell;

typedef enum {
    P_RHOMBOID,
    P_CROOK,
    P_CROWN,
    P_SPHINX,
    P_SNAKE,
    P_YACHT,
    P_BAT,
    P_PISTOL,
    P_LOBSTER,
    P_HOOK,
    P_HEXAGON,
    P_BUTTERFLY,
} PieceType;

typedef struct {
    PieceType n;
    Cell c[6];
} Piece;

size_t piece_hash(void *item, size_t size) 
{
    (void) size;

    Piece *p = item;
    size_t h = 1469598103934665603ULL;

    for (size_t i = 0; i < ARRAY_LEN(p->c); i++) 
    {
        h ^= p->c[i].x; h *= 1099511628211ULL;
        h ^= p->c[i].y; h *= 1099511628211ULL;
        h ^= p->c[i].z; h *= 1099511628211ULL;
    }

    return h;
}

int piece_cmp(const Piece *a, const Piece *b)
{
    if (a->n != b->n) return (int)a->n - (int)b->n;

    for (size_t i = 0; i < ARRAY_LEN(a->c); ++i)
    {
        if (a->c[i].x != b->c[i].x) return a->c[i].x - b->c[i].x;

        if (a->c[i].y != b->c[i].y) return a->c[i].y - b->c[i].y;

        if (a->c[i].z != b->c[i].z) return a->c[i].z - b->c[i].z;
    }

    return 0;
}

bool piece_equal(void *a, size_t as, void *b, size_t bs)
{
    (void) as;
    (void) bs;

    return piece_cmp((Piece*)a, (Piece*)b) == 0;
}

#define HASHSET_IMPLEMENTATION
#define hashset_hash piece_hash
#define hashset_equal piece_equal
#include "../hashset.h"

static char *pieces[] = {
    "U00 D01 U02 D03 U04 D05", // rhomboid
    "D00 U10 D11 U12 D13 U14", // crook
    "D02 U10 D11 U12 D13 U14", // crown
    "D11 U00 D01 U02 D03 U04", // sphinx
    "D00 U10 D11 U12 D13 U23", // snake
    "U00 D01 U02 D03 U11 U13", // yacht
    "U00 D01 U02 D03 U11 D12", // bat
    "U00 D01 U02 D03 D10 U11", // pistol
    "D00 D02 U10 D11 U12 U21", // lobster
    "U00 D01 D03 U11 D12 U13", // hook
    "D00 U01 D02 U10 D11 U12", // hexagon
    "U00 D01 U02 D10 U11 D12", // butterfly
};

void piece_parse(Piece *p, char *repr)
{
    static PieceType type = P_RHOMBOID;
    char buffer[128];
    memcpy(buffer, repr, strlen(repr));

    p->n = type++;

    size_t i = 0;
    char *tok = strtok(buffer, " ");
    while(tok)
    {
        p->c[i].x = tok[1] - '0';
        p->c[i].y = tok[2] - '0';
        p->c[i].z = tok[0] == 'D';

        i++;
        tok = strtok(NULL, " ");
    }
}

int cell_cmp(const void *a, const void *b) 
{
    Cell *A = (Cell *)a;
    Cell *B = (Cell *)b;

    if (A->x != B->x) return A->x - B->x;
    if (A->y != B->y) return A->y - B->y;

    return A->z - B->z;
}

void piece_normalize(Piece *p)
{
    int xmin = INT_MAX;
    int ymin = INT_MAX;

    for(size_t i = 0; i < ARRAY_LEN(p->c); ++i)
    {
        if(p->c[i].x < xmin) xmin = p->c[i].x;
        if(p->c[i].y < ymin) ymin = p->c[i].y;
    }

    for(size_t i = 0; i < ARRAY_LEN(p->c); ++i)
    {
        p->c[i].x -= xmin;
        p->c[i].y -= ymin;
    }

    qsort(p->c, ARRAY_LEN(p->c), sizeof(Cell), cell_cmp);
}

Piece piece_offset(const Piece piece, const int x, const int y)
{
    Piece ret;
    memcpy(&ret, &piece, sizeof(Piece));

    for (size_t i = 0; i < ARRAY_LEN(ret.c); ++i)
    {
        ret.c[i].x += x;
        ret.c[i].y += y;
    }

    return ret;
}

void piece_rotate60(Piece *p) 
{
    for (size_t i = 0; i < ARRAY_LEN(p->c); i++) 
    {
        const int x = p->c[i].x;
        const int y = p->c[i].y;
        const int z = p->c[i].z;
        p->c[i].x = x + y + z;
        p->c[i].y = -x;
        p->c[i].z = 1 - z;
    }
}

bool piece_fits(const Piece p)
{
    (void) p;

    assert(0 && "TODO");

    return false;
}

void piece_dump(const Piece p)
{
    (void) p;
    assert(0 && "TODO");
}

Piece piece_refect(const Piece p)
{
    Piece ret;
    memcpy(&ret, &p, sizeof(Piece));

    assert(0 && "TODO");

    return ret;
}

void build_matrix(void)
{
    printf("%d %d %d\n", (int) NROWS, (int) NCOLS, (int) NMAND);

    for(size_t p = 0; p < NUM_PIECES; ++p)
    {
        Piece base = {0};
        piece_parse(&base, pieces[p]);

        hashset_t hs = {0};

        for(int r = 0; r < 6; ++r)
        {
            Piece rot = base;
            piece_normalize(&rot);

            for(int flip = 0; flip < 2; ++flip)
            {
                Piece orient = rot;
                if(flip) orient = piece_refect(orient);
                piece_normalize(&orient);

                for(int y = 0; y < HEIGHT; ++y)
                {
                    for(int x = 0; x < WIDTH; ++x)
                    {
                        Piece placed = piece_offset(orient, y, x);
                        
                        if(piece_fits(placed))
                        {
                            if(!hashset_contains(&hs, &placed, sizeof(Piece)))
                            {
                                piece_dump(placed); 
                                hashset_insert(&hs, &placed, sizeof(Piece));
                            }
                        }
                    }
                }
            }

            piece_rotate60(&rot);
        }

        hashset_free(&hs);
    }
}

int main(void)
{
    assert(0 && "Work in progress. Do not use!");
    build_matrix();

    return 0;
}
