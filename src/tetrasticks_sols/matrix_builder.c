#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define HASHSET_INIT_CAP 256
#define HASHSET_IMPLEMENTATION
#include "hashset.h"

#define ARRAY_LEN(a)(sizeof(a)/sizeof(a[0]))
#define WIDTH 6
#define HEIGHT 6

typedef struct
{
    int x, y;
} Coord;

typedef enum
{
    I,
    // L,
    Y,
    N,
    F,
    H,
    J,
    X,
    P,
    U,
    T,
    R,
    Z,
    O,
    W,
    V,
} PieceKind;

typedef struct
{
    PieceKind kind;
    Coord v[WIDTH * HEIGHT]; size_t vc;
    Coord h[WIDTH * HEIGHT]; size_t hc;
    Coord i[WIDTH * HEIGHT]; size_t ic;
} Piece;

static char *pieces[] = {
    "V00 V01 V02 V03 I01 I02 I03", // I
    // "V00 V01 V02 H00 I01 I02",     // L
    "V00 V01 V02 H01 I02",         // Y
    "V00 V01 V12 H02 I01",         // N
    "V00 V01 H01 H02",             // F
    "V00 V01 V10 H01",             // H
    "V00 V10 V11 H00 I11",         // J
    "V10 V11 H01 H11",             // X
    "V00 V11 H01 H02",             // P
    "V00 V20 H00 H10 I10",         // U
    "V10 V11 H02 H12 I11",         // T
    "V10 V11 H01 H12",             // R
    "V10 V11 H00 H12 I11",         // Z
    "V00 V10 H00 H01",             // O
    "V10 V21 H00 H11",             // W
    "V00 V01 H00 H10 I01 I10",     // V
};

#define NUM_PIECES (ARRAY_LEN(pieces))
#define NUM_H (WIDTH * (HEIGHT - 1))
#define NUM_V ((WIDTH - 1) * HEIGHT)
#define NUM_I ((WIDTH - 2) * (HEIGHT - 2))

#define PIECE_OFFSET 0
#define HEDGE_OFFSET (PIECE_OFFSET + NUM_PIECES)
#define VEDGE_OFFSET (HEDGE_OFFSET + NUM_H)
#define INODE_OFFSET (VEDGE_OFFSET + NUM_V)

#define NMAND (NUM_PIECES + NUM_H + NUM_V)
#define NCOLS (NMAND + NUM_I)
#define NROWS 1085

int H_index(const Coord c)
{
    return HEDGE_OFFSET + c.y * (WIDTH - 1) + c.x;
}

int V_index(const Coord c)
{
    return VEDGE_OFFSET + c.y * WIDTH + c.x;
}

int I_index(const Coord c)
{
    return INODE_OFFSET + (c.y - 1) * (WIDTH - 2) + (c.x - 1);
}

void dump_piece(const Piece p)
{
    printf("%d ", p.kind);

    for (size_t h = 0; h < p.hc; ++h)
    {
        printf("%d ", H_index(p.h[h]));
    }

    for (size_t v = 0; v < p.vc; ++v)
    {
        printf("%d ", V_index(p.v[v]));
    }

    for (size_t i = 0; i < p.ic; ++i)
    {
        printf("%d ", I_index(p.i[i]));
    }

    printf("\n");
}

void parse_piece(Piece *p, const char *repr)
{
    static PieceKind kind = I; // TODO make this better
    p->kind               = kind++;
    char buf[128];
    strcpy(buf, repr);

    const char *tok = strtok(buf, " ");
    while (tok)
    {
        const char type = tok[0];
        const int x     = tok[1] - '0';
        const int y     = tok[2] - '0';

        switch (type)
        {
            case 'H': p->h[p->hc++] = (Coord){x, y};
                break;
            case 'V': p->v[p->vc++] = (Coord){x, y};
                break;
            case 'I': p->i[p->ic++] = (Coord){x, y};
                break;
            default: break;
        }

        tok = strtok(NULL, " ");
    }
}

Piece piece_rotate90(const Piece p)
{
    Piece ret = {.kind = p.kind};

    for (size_t v = 0; v < p.vc; ++v)
    {
        const Coord rot                         = p.v[v];
        ret.h[ret.hc++] = (Coord){rot.y, -rot.x};
    }

    for (size_t h = 0; h < p.hc; ++h)
    {
        const Coord rot                     = p.h[h];
        ret.v[ret.vc++] = (Coord){rot.y, -rot.x - 1};
    }

    for (size_t i = 0; i < p.ic; ++i)
    {
        const Coord rot                    = p.i[i];
        ret.i[ret.ic++] = (Coord){rot.y, -rot.x};
    }

    return ret;
}

Piece piece_reflect(const Piece p)
{
    Piece ret = {.kind = p.kind};

    for (size_t h = 0; h < p.hc; ++h)
    {
        const Coord rot                         = p.h[h];
        ret.h[ret.hc++] = (Coord){-rot.x - 1, rot.y};
    }

    for (size_t v = 0; v < p.vc; ++v)
    {
        const Coord rot                     = p.v[v];
        ret.v[ret.vc++] = (Coord){-rot.x, rot.y};
    }

    for (size_t i = 0; i < p.ic; ++i)
    {
        const Coord rot                    = p.i[i];
        ret.i[ret.ic++] = (Coord){-rot.x, rot.y};
    }

    return ret;
}

bool piece_fits(const Piece p)
{
    for (size_t i = 0; i < p.hc; ++i)
    {
        const Coord c = p.h[i];
        if (c.x < 0 || c.x >= WIDTH - 1 || c.y < 0 || c.y >= HEIGHT) return false;
    }

    for (size_t i = 0; i < p.vc; ++i)
    {
        const Coord c = p.v[i];
        if (c.x < 0 || c.x >= WIDTH || c.y < 0 || c.y >= HEIGHT - 1) return false;
    }

    for (size_t i = 0; i < p.ic; ++i)
    {
        const Coord c = p.i[i];
        if (c.x <= 0 || c.x >= WIDTH - 1 || c.y <= 0 || c.y >= HEIGHT - 1) return false;
    }

    return true;
}

Piece piece_offset(const Piece *p, const int y, const int x)
{
    Piece ret = {.kind = p->kind};

    for (size_t h = 0; h < p->hc; ++h)
    {
        const Coord c                           = p->h[h];
        ret.h[ret.hc++] = (Coord){c.x + x, c.y + y};
    }

    for (size_t v = 0; v < p->vc; ++v)
    {
        const Coord c                       = p->v[v];
        ret.v[ret.vc++] = (Coord){c.x + x, c.y + y};
    }

    for (size_t i = 0; i < p->ic; ++i)
    {
        const Coord c                      = p->i[i];
        ret.i[ret.ic++] = (Coord){c.x + x, c.y + y};
    }

    return ret;
}

Piece filter_interiors(const Piece *p)
{
    Piece ret;
    memcpy(&ret, p, sizeof(Piece));
    ret.ic = 0;

    for (size_t i = 0; i < p->ic; ++i)
    {
        const Coord c = p->i[i];
        if (c.x != 0 && c.x != WIDTH - 1 && c.y != 0 && c.y != HEIGHT - 1)
        {
            ret.i[ret.ic++] = c;
        }
    }

    return ret;
}

int cmp_coord(const void *a, const void *b)
{
    const Coord *A = a;
    const Coord *B = b;
    if (A->y != B->y) return A->y - B->y;
    return A->x - B->x;
}

void normalize_piece(Piece *p)
{
    int min_x = INT_MAX, min_y = INT_MAX;

    for (size_t i = 0; i < p->hc; ++i)
    {
        if (p->h[i].x < min_x) min_x = p->h[i].x;
        if (p->h[i].y < min_y) min_y = p->h[i].y;
    }

    for (size_t i = 0; i < p->vc; ++i)
    {
        if (p->v[i].x < min_x) min_x = p->v[i].x;
        if (p->v[i].y < min_y) min_y = p->v[i].y;
    }

    for (size_t i = 0; i < p->ic; ++i)
    {
        if (p->i[i].x < min_x) min_x = p->i[i].x;
        if (p->i[i].y < min_y) min_y = p->i[i].y;
    }

    for (size_t i = 0; i < p->hc; ++i)
    {
        p->h[i].x -= min_x;
        p->h[i].y -= min_y;
    }

    for (size_t i = 0; i < p->vc; ++i)
    {
        p->v[i].x -= min_x;
        p->v[i].y -= min_y;
    }

    for (size_t i = 0; i < p->ic; ++i)
    {
        p->i[i].x -= min_x;
        p->i[i].y -= min_y;
    }

    qsort(p->h, p->hc, sizeof(Coord), cmp_coord);
    qsort(p->v, p->vc, sizeof(Coord), cmp_coord);
    qsort(p->i, p->ic, sizeof(Coord), cmp_coord);
}

void build_matrix(void)
{
    printf("%d %d %d\n", (int) NROWS, (int) NCOLS, (int) NMAND);

    for (size_t p = 0; p < NUM_PIECES; ++p)
    {
        Piece base = {0};
        parse_piece(&base, pieces[p]);

        hashset_t hs = {0};

        for (int r = 0; r < 4; ++r)
        {
            Piece rot = base;
            normalize_piece(&rot);

            for (int flip = 0; flip < 2; ++flip)
            {
                Piece orient = rot;

                if (flip) orient = piece_reflect(orient);

                normalize_piece(&orient);

                for (int y = 0; y < HEIGHT; ++y)
                {
                    for (int x = 0; x < WIDTH; ++x)
                    {
                        Piece placed = piece_offset(&orient, y, x);
                        Piece filtered = filter_interiors(&placed);

                        if (piece_fits(filtered))
                        {
                            if(!hashset_contains(&hs, &filtered, sizeof(Piece)))
                            {
                                dump_piece(filtered);
                                hashset_insert(&hs, &filtered, sizeof(Piece));
                            } 
                        }
                    }
                }
            }

            base = piece_rotate90(base);
        }

        hashset_free(&hs);
    }
}

int main(void)
{
    build_matrix();
    return 0;
}
