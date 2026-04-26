#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define WIDTH 6
#define HEIGHT 6

typedef struct {
    int x, y;
} Coord;

typedef struct {
    int kind;

    Coord h[WIDTH * HEIGHT]; size_t hc;
    Coord v[WIDTH * HEIGHT]; size_t vc;
    Coord i[WIDTH * HEIGHT]; size_t ic;
} Piece;

int cmp_piece(const void *a, const void *b);

bool hs_piece_cmp(const void *a, const size_t as, const void *b, const size_t bs)
{
    (void) as;
    (void) bs;
    return cmp_piece(a, b) == 0;
}

size_t hs_piece_hash(const void *a, const size_t as)
{
    (void) as;

    const Piece *piece = a;

    static const size_t magic_prime = 0x00000100000001b3;
    size_t hash = 0xcbf29ce484222325;
    
    hash = (hash ^ piece->kind) * magic_prime;

    for(size_t i = 0; i < piece->hc; ++i)
    {
        hash = (hash ^ piece->h[i].x) * magic_prime;
        hash = (hash ^ piece->h[i].y) * magic_prime;
    }

    for(size_t i = 0; i < piece->vc; ++i)
    {
        hash = (hash ^ piece->v[i].x) * magic_prime;
        hash = (hash ^ piece->v[i].y) * magic_prime;
    }

    for(size_t i = 0; i < piece->ic; ++i)
    {
        hash = (hash ^ piece->i[i].x) * magic_prime;
        hash = (hash ^ piece->i[i].y) * magic_prime;
    }

    return hash;
}

#define HASHSET_EQUAL hs_piece_cmp
#define HASHSET_HASH hs_piece_hash
#define HASHSET_IMPLEMENTATION
#include "hashset.h"

#define shift_args(argc, argv) ((argc)--, *(argv)++)

#define NUM_PIECES 15
#define NUM_H (WIDTH * (HEIGHT - 1))
#define NUM_V ((WIDTH - 1) * HEIGHT)
#define NUM_I ((WIDTH - 2) * (HEIGHT - 2))

#define PIECE_OFFSET 0
#define HEDGE_OFFSET (PIECE_OFFSET + NUM_PIECES)
#define VEDGE_OFFSET (HEDGE_OFFSET + NUM_H)
#define INODE_OFFSET (VEDGE_OFFSET + NUM_V)

#define SCALE 40
#define MARGIN 20
#define THICKNESS 3

static const int colors[][3] = {
    {255,   0,   0},
    {  0, 255,   0},
    {  0,   0, 255},
    {255, 255,   0},
    {255,   0, 255},
    {  0, 255, 255},
    {255, 128,   0},
    {128,   0, 255},
    {  0, 128, 255},
    {128, 255,   0},
    {255,   0, 128},
    {  0, 255, 128},
    {255,  64,  64},
    { 64, 255,  64},
    { 64,  64, 255},
    {255, 200,   0},
};

#define NUM_COLORS (sizeof(colors)/sizeof(colors[0]))

int abs(int a){ return a < 0 ? -a : a; }

Coord H_decode(int idx)
{
    idx -= HEDGE_OFFSET;
    return (Coord){
        .x = idx % (WIDTH - 1),
        .y = idx / (WIDTH - 1)
    };
}

Coord V_decode(int idx)
{
    idx -= VEDGE_OFFSET;
    return (Coord){
        .x = idx % WIDTH,
        .y = idx / WIDTH
    };
}

Coord I_decode(int idx)
{
    idx -= INODE_OFFSET;
    return (Coord){
        .x = (idx % (WIDTH - 2)) + 1,
        .y = (idx / (WIDTH - 2)) + 1
    };
}

void draw_solution(Piece *pieces, int count, const char *filename)
{
    const int img_w = (WIDTH - 1) * SCALE + 2 * MARGIN;
    const int img_h = (HEIGHT - 1) * SCALE + 2 * MARGIN;

    FILE *f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "P3\n%d %d\n255\n", img_w, img_h);

    for(int y = 0; y < img_h; y++)
    {
        for(int x = 0; x < img_w; x++)
        {
            int r = 240, g = 240, b = 240;

            for(int p = 0; p < count; p++)
            {
                Piece *pc = &pieces[p];
                const int *col = colors[pc->kind % NUM_COLORS];

                for(size_t i = 0; i < pc->hc; i++)
                {
                    int sx = MARGIN + pc->h[i].x * SCALE;
                    int sy = MARGIN + pc->h[i].y * SCALE;

                    if(x >= sx && x <= sx + SCALE && abs(y - sy) <= THICKNESS)
                    {
                        r = col[0] / 2;
                        g = col[1] / 2;
                        b = col[2] / 2;
                    }
                }

                for(size_t i = 0; i < pc->vc; i++)
                {
                    int sx = MARGIN + pc->v[i].x * SCALE;
                    int sy = MARGIN + pc->v[i].y * SCALE;

                    if(y >= sy && y <= sy + SCALE && abs(x - sx) <= THICKNESS)
                    {
                        r = col[0] / 2;
                        g = col[1] / 2;
                        b = col[2] / 2;
                    }
                }

                for(size_t i = 0; i < pc->ic; i++)
                {
                    int ix = MARGIN + pc->i[i].x * SCALE;
                    int iy = MARGIN + pc->i[i].y * SCALE;

                    if(abs(x - ix) <= 4 && abs(y - iy) <= 4)
                    {
                        r = g = b = 0;
                    }
                }
            }

            fprintf(f, "%d %d %d ", r, g, b);
        }
        fprintf(f, "\n");
    }

    fclose(f);
}


int cmp_piece(const void *a, const void *b)
{
    const Piece *A = a;
    const Piece *B = b;

    if (A->kind != B->kind)
        return A->kind - B->kind;

    if (A->hc != B->hc) return A->hc - B->hc;
    int r = memcmp(A->h, B->h, A->hc * sizeof(Coord));
    if (r) return r;

    if (A->vc != B->vc) return A->vc - B->vc;
    r = memcmp(A->v, B->v, A->vc * sizeof(Coord));
    if (r) return r;

    if (A->ic != B->ic) return A->ic - B->ic;
    return memcmp(A->i, B->i, A->ic * sizeof(Coord));
}

int cmp_coord(const void *a, const void *b)
{
    const Coord *A = a;
    const Coord *B = b;
    if (A->y != B->y) return A->y - B->y;
    return A->x - B->x;
}

int main(int argc, char **argv)
{
    hashset_t hs = {0};

    char line[1024];

    Piece solution[64];
    int sol_count = 0;
    int sol_id = 0;

    char prefix[256] = {0};
    if(argc >= 2)
    {
        strncpy(prefix, argv[1], sizeof(prefix) - 1);
        const size_t len = strlen(prefix);
        if(prefix[len - 1] != '/')
        {
            prefix[len] = '/';
            prefix[len + 1] = 0;
        }
    }

    while(fgets(line, sizeof(line), stdin))
    {
        if(strlen(line) <= 1)
        {
        
            for (int i = 0; i < sol_count; i++) 
            {
                qsort(solution[i].h, solution[i].hc, sizeof(Coord), cmp_coord);
                qsort(solution[i].v, solution[i].vc, sizeof(Coord), cmp_coord);
                qsort(solution[i].i, solution[i].ic, sizeof(Coord), cmp_coord);
            }

            qsort(solution, sol_count, sizeof(Piece), cmp_piece);

            const size_t size = sol_count * sizeof(Piece);
            if(!hashset_contains(&hs, solution, size))
            {
                char name[1024];
                snprintf(name, sizeof(name), "%ssolution_%d.ppm", prefix, sol_id++);
                draw_solution(solution, sol_count, name);

                hashset_insert(&hs, solution, size);
            }

            sol_count = 0;
            continue;
        }

        Piece p = {0};

        const char *tok = strtok(line, " \n");
        while(tok)
        {
            const int idx = atoi(tok);

            if(idx < NUM_PIECES)
            {
                p.kind = idx;
            }
            else if(idx < VEDGE_OFFSET)
            {
                p.h[p.hc++] = H_decode(idx);
            }
            else if(idx < INODE_OFFSET)
            {
                p.v[p.vc++] = V_decode(idx);
            }
            else
            {
                p.i[p.ic++] = I_decode(idx);
            }

            tok = strtok(NULL, " \n");
        }

        solution[sol_count++] = p;
    }

    hashset_free(&hs);

    return 0;
}
