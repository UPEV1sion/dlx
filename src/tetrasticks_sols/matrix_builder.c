#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define ARRAY_LEN(a)(sizeof(a)/sizeof(a[0]))
#define WIDTH 5
#define HEIGHT 5

typedef struct {
    int x, y;
} Coord;

typedef enum {
    I,
    L,
    Y,
    N,
    F,
    H,
    J,
    X,
    P,
    U,
    R,
    Z,
    O,
    W,
    V,
} PieceKind;

typedef struct {
    PieceKind kind;
    Coord verticals[WIDTH * HEIGHT];
    size_t vertical_count;
    Coord horizontals[WIDTH * HEIGHT];
    size_t horizontal_count;
    Coord interior[WIDTH * HEIGHT];
    size_t interior_count;
} Piece;

static char *pieces[] = {
    "V00 V01 V02 V03 I01 I02 I03", // I
    // "V00 V01 V02 H00 I01 I02",     // L
    "V00 V01 V02 H01 I02",         // Y
    "V00 V01 V12 H02 I01",         // N
    "V00 V01 H01 H02",             // F
    // "V00 V01 V10 H01",             // H
    "V00 V10 V11 H00 I11",         // J
    "V10 V11 H01 H11",             // X
    "V00 V11 H01 H02",             // P
    "V00 V20 H00 H10 I10",         // U
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
    return HEDGE_OFFSET + c.y * (WIDTH-1) + c.x;
}

int V_index(const Coord c)
{
    return VEDGE_OFFSET + c.y * WIDTH + c.x;
}

int I_index(const Coord c)
{
    return INODE_OFFSET + (c.y-1)*(WIDTH-2) + (c.x-1);
}

void dump_piece(const Piece p)
{
    printf("%d ", p.kind);

    for(size_t h = 0; h < p.horizontal_count; ++h)
    {
        printf("%d ", H_index(p.horizontals[h]));
    }

    for(size_t v = 0; v < p.vertical_count; ++v)
    {
        printf("%d ", V_index(p.verticals[v]));
    }

    for(size_t i = 0; i < p.interior_count; ++i)
    {
        printf("%d ", I_index(p.interior[i]));
    }

    printf("\n");
}

void parse_piece(Piece *p, const char *repr)
{
    static PieceKind kind = I; // TODO make this better
    p->kind = kind++;
    char buf[128];
    strcpy(buf, repr);

    const char *tok = strtok(buf, " ");
    while(tok)
    {
        const char type = tok[0];
        const int x = tok[1] - '0';
        const int y = tok[2] - '0';

        switch(type)
        {
            case 'H': p->horizontals[p->horizontal_count++] = (Coord) {x, y}; break;
            case 'V': p->verticals[p->vertical_count++] = (Coord) {x, y}; break;
            case 'I': p->interior[p->interior_count++] = (Coord) {x, y}; break;
            default: break;
        }

        tok = strtok(NULL, " ");
    }
}

Piece piece_rotate90(const Piece p)
{
    Piece ret = {.kind = p.kind};

    for(size_t v = 0; v < p.vertical_count; ++v)
    {
        const Coord rot = p.verticals[v];
        ret.horizontals[ret.horizontal_count++] = (Coord) {rot.y, -rot.x};
    }

    for(size_t h = 0; h < p.horizontal_count; ++h)
    {
        const Coord rot = p.horizontals[h];
        ret.verticals[ret.vertical_count++] = (Coord) {rot.y, -rot.x - 1};
    }

    for(size_t i = 0; i < p.interior_count; ++i)
    {
        const Coord rot = p.interior[i];
        ret.interior[ret.interior_count++] = (Coord) {rot.y, -rot.x};
    }

    return ret;
}

Piece piece_reflect(const Piece p)
{
    Piece ret = {.kind = p.kind};

    for(size_t h = 0; h < p.horizontal_count; ++h)
    {
        const Coord rot = p.horizontals[h];
        ret.horizontals[ret.horizontal_count++] = (Coord) {-rot.x - 1, rot.y};
    }

    for(size_t v = 0; v < p.vertical_count; ++v)
    {
        const Coord rot = p.verticals[v];
        ret.verticals[ret.vertical_count++] = (Coord) {-rot.x, rot.y};
    }

    for(size_t i = 0; i < p.interior_count; ++i)
    {
        const Coord rot = p.interior[i];
        ret.interior[ret.interior_count++] = (Coord) {-rot.x, rot.y};
    }

    return ret;
}

bool piece_fits(const Piece p)
{
    for(size_t i = 0; i < p.horizontal_count; ++i) {
        const Coord c = p.horizontals[i];
        if(c.x < 0 || c.x >= WIDTH - 1 || c.y < 0 || c.y >= HEIGHT) return false;
    }

    for(size_t i = 0; i < p.vertical_count; ++i) {
        const Coord c = p.verticals[i];
        if(c.x < 0 || c.x >= WIDTH || c.y < 0 || c.y >= HEIGHT - 1) return false;
    }

    for(size_t i = 0; i < p.interior_count; ++i) {
        const Coord c = p.interior[i];
        if(c.x <= 0 || c.x >= WIDTH - 1 || c.y <= 0 || c.y >= HEIGHT - 1) return false;
    }

    return true;
}

Piece piece_offset(const Piece *p, const int y, const int x)
{
    Piece ret = {.kind = p->kind};

    for(size_t h = 0; h < p->horizontal_count; ++h)
    {
        const Coord c = p->horizontals[h];
        ret.horizontals[ret.horizontal_count++] = (Coord) {c.x + x, c.y + y};
    }

    for(size_t v = 0; v < p->vertical_count; ++v)
    {
        const Coord c = p->verticals[v];
        ret.verticals[ret.vertical_count++] = (Coord) {c.x + x, c.y + y};
    }

    for(size_t i = 0; i < p->interior_count; ++i)
    {
        const Coord c = p->interior[i];
        ret.interior[ret.interior_count++]  = (Coord) {c.x + x, c.y + y};
    }

    return ret;
}

Piece filter_interiors(const Piece *p)
{
    Piece ret = {.kind = p->kind};
    memcpy(&ret, p, sizeof(Piece));
    ret.interior_count = 0;

    for(size_t i = 0; i < p->interior_count; ++i)
    {
        const Coord c = p->interior[i];
        if(!(c.x == 0 || c.x == WIDTH - 1 || c.y == 0 || c.y == HEIGHT - 1))
        {
            ret.interior[ret.interior_count++] = c;
        }
    }

    return ret;
}

void normalize_piece(Piece *p)
{
    int min_x = INT_MAX, min_y = INT_MAX;

    for(size_t i = 0; i < p->horizontal_count; ++i)
    {
        if(p->horizontals[i].x < min_x) min_x = p->horizontals[i].x;
        if(p->horizontals[i].y < min_y) min_y = p->horizontals[i].y;
    }

    for(size_t i = 0; i < p->vertical_count; ++i)
    {
        if(p->verticals[i].x < min_x) min_x = p->verticals[i].x;
        if(p->verticals[i].y < min_y) min_y = p->verticals[i].y;
    }

    for(size_t i = 0; i < p->interior_count; ++i)
    {
        if(p->interior[i].x < min_x) min_x = p->interior[i].x;
        if(p->interior[i].y < min_y) min_y = p->interior[i].y;
    }

    for(size_t i = 0; i < p->horizontal_count; ++i)
    {
        p->horizontals[i].x -= min_x;
        p->horizontals[i].y -= min_y;
    }

    for(size_t i = 0; i < p->vertical_count; ++i)
    {
        p->verticals[i].x -= min_x;
        p->verticals[i].y -= min_y;
    }

    for(size_t i = 0; i < p->interior_count; ++i)
    {
        p->interior[i].x -= min_x;
        p->interior[i].y -= min_y;
    }
}

void build_matrix(void)
{
    printf("%d %d %d\n", (int) NROWS, (int) NCOLS, (int) NMAND);

    for(int p = 0; p < NUM_PIECES; ++p)
    {
        Piece base = {0};
        parse_piece(&base, pieces[p]);

        for(int r = 0; r < 4; ++r)
        {
            Piece rot = base;
            normalize_piece(&rot);

            for(int flip = 0; flip < 2; ++flip)
            {
                Piece orient = rot;

                if(flip) orient = piece_reflect(orient);

                normalize_piece(&orient);

                for(int y = 0; y < HEIGHT; ++y)
                {
                    for(int x = 0; x < WIDTH; ++x)
                    {
                        Piece placed = piece_offset(&orient, y, x);
                        const Piece filtered = filter_interiors(&placed);

                        if(piece_fits(filtered))
                        {
                            dump_piece(filtered);
                        }
                    }
                }
            }

            base = piece_rotate90(base);
        }
    }
}

int main(void)
{
    build_matrix();
    return 0;
}
