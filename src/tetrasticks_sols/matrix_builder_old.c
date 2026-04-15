#include <stdio.h>
#include <string.h>

#define PIECE_LEN 5
#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))

typedef struct {
    int x, y;
} Coord;

typedef struct {
    Coord coords[PIECE_LEN];
} Piece;

static Piece pieces[] = {
    {{{0,0}, {0,1}, {0,2}, {0,3}, {0,4}}}, // I
    {{{0,0}, {0,1}, {0,2}, {0,3}, {1,0}}}, // L
    {{{0,0}, {0,1}, {0,2}, {1,2}, {1,3}}}, // Z
    {{{0,0}, {0,1}, {0,2}, {1,1}, {1,2}}}, // F
    {{{0,0}, {0,1}, {0,2}, {1,1}, {1,0}}}, // H
    {{{0,0}, {0,1}, {1,0}, {1,1}, {1,2}}}, // J
    {{{0,1}, {1,1}, {2,1}, {1,0}, {1,2}}}, // X
    {{{1,0}, {1,1}, {0,1}, {0,2}, {1,2}}}, // P
    {{{0,0}, {0,1}, {1,0}, {0,2}, {1,2}}}, // U
    {{{1,0}, {1,1}, {1,2}, {0,2}, {2,2}}}, // T
    {{{1,0}, {0,1}, {1,1}, {1,2}, {2,2}}}, // Y
    {{{0,0}, {1,0}, {1,1}, {1,2}, {2,2}}}, // Z
    {{{0,0}, {1,0}, {1,1}, {0,1}, {0,0}}}, // O
    {{{0,0}, {1,0}, {1,1}, {2,1}, {2,2}}}, // W
    {{{0,0}, {0,1}, {0,2}, {1,0}, {2,0}}}, // V
};

int main(void)
{
    for(size_t i = 0; i < ARRAY_LEN(pieces); ++i)
    {
        Piece piece = pieces[i];
        char buf[5][5];
        memset(buf, ' ', sizeof(buf));
        for(size_t j = 0; j < ARRAY_LEN(piece.coords); ++j)
        {
            Coord coord = piece.coords[j];
            buf[coord.y][coord.x] = '#';
        }

        for(size_t y = 6; y > 0; --y)
        {
            for(size_t x = 0; x < 5; ++x)
            {
                printf("%c", buf[y - 1][x]);
            }
            printf("\n");
        }
    }

    return 0;
}
