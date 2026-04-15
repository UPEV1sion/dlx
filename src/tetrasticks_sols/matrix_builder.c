#include <stdio.h>
#include <string.h>

#define ARRAY_LEN(a)(sizeof(a)/sizeof(a[0]))

typedef enum {
    O_HOR,
    O_VER,
} Orient;

typedef struct {
    int x, y;
    Orient orient;
} Stick;

typedef struct {
    Stick sticks[4];
} Piece;

// static Piece pieces[] = {
//     {{{0,0,O_VER}, {0,1,O_VER}, {0,2,O_VER}, {0,3,O_VER}}}, // I
//     {{{0,0,O_VER}, {0,1,O_VER}, {0,2,O_VER}, {0,0,O_HOR}}}, // L
//     {{{0,0,O_VER}, {0,1,O_VER}, {0,2,O_VER}, {0,1,O_HOR}}}, // Y
//     {{{0,0,O_VER}, {0,1,O_VER}, {0,2,O_HOR}, {1,2,O_VER}}}, // N
//     {{{0,0,O_VER}, {0,1,O_VER}, {0,1,O_HOR}, {0,2,O_HOR}}}, // F
//     {{{0,0,O_VER}, {0,1,O_VER}, {0,1,O_HOR}, {1,0,O_VER}}}, // H
//     {{{0,0,O_VER}, {0,0,O_HOR}, {1,0,O_VER}, {1,1,O_VER}}}, // J
//     {{{1,0,O_VER}, {1,1,O_VER}, {0,1,O_HOR}, {1,1,O_HOR}}}, // X
// };

static char *pieces[] = {
    "V00 V01 V02 V03 I01 I02 I03", // I
    "V00 V01 V02 H00 I01 I02",     // L
    "V00 V01 V02 H01 I02",         // Y
    "V00 V01 V12 H02 I01",         // N
    "V00 V01 H01 H02",             // F
    "V00 V01 V10 H01",             // H
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

static char *names = "ILYNFHJXPURZOWV";

int main(void)
{
    for(size_t i = 0; i < ARRAY_LEN(pieces); ++i)
    {
        char grid[7][7];
        memset(grid, ' ', sizeof(grid));
        
        char buf[128];
        strcpy(buf, pieces[i]);
        char *tok = strtok(buf, " ");
        while(tok)
        {
            char type = tok[0];
            int x = (tok[1] - '0') * 2;
            int y = (tok[2] - '0') * 2;

            if(type == 'V')
            {
                grid[y][x] = '|';
                grid[y+1][x] = '|';
            }
            else if(type == 'H')
            {
                grid[y][x] = '-';
                grid[y][x+1] = '-';
            }
            else
            {
                grid[y][x] = '+';
            }

            tok = strtok(NULL, " ");
        }

        putchar(names[i]);
        for (int y = 6; y >= 0; --y)
        {
            for (int x = 0; x < 7; ++x)
            {
                putchar(grid[y][x]);
            }
            putchar('\n');
        }

        printf("\n\n");
    }

    return 0;
}
