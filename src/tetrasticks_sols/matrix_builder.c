#include <stdio.h>
#include <string.h>

#define ARRAY_LEN(a)(sizeof(a)/sizeof(a[0]))

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

#define W 5
#define H 5
#define NUM_PIECES (ARRAY_LEN(pieces))
#define NUM_H (W * (H - 1))
#define NUM_V ((W - 1) * H)
#define NUM_I ((W - 2) * (H - 2))

#define PIECE_OFFSET 0
#define HEDGE_OFFSET PIECE_OFFSET + NUM_PIECES
#define VEDGE_OFFSET HEDGE_OFFSET + NUM_H
#define INODE_OFFSET VEDGE_OFFSET + NUM_V

#define NMAND NUM_PIECES + NUM_H + NUM_V
#define NCOLS NMAND + NUM_I

int H_index(const int x, const int y)
{
    return HEDGE_OFFSET + y * (W-1) + x;
}

int V_index(const int x, const int y)
{
    return VEDGE_OFFSET + y * W + x;
}

int I_index(const int x, const int y) 
{
    return INODE_OFFSET + (y-1)*(W-2) + (x-1);
}

void emit_row(int *cols, int count)
{
    for(int i = 0; i < count; ++i)
    {
        printf("%d ", cols[i]);
    }
    printf("\n");
}

void build_matrix(void)
{
    // TODO
    printf("%d %d %d\n", (int) NUM_PIECES, (int) NCOLS, (int) NMAND); 

    for(int p = 0; p < NUM_PIECES; ++p)
    {
        char buf[128];
        strcpy(buf, pieces[p]);

        int cols[64];
        int count = 0;

        cols[count++] = PIECE_OFFSET + p;

        int H_used[W][H] = {0};
        int V_used[W][H] = {0};

        char *tok = strtok(buf, " ");
        while(tok)
        {
            char type = tok[0];
            int x = tok[1] - '0';
            int y = tok[2] - '0';

            switch(type)
            {
                case 'H': cols[count++] = H_index(x, y); break;
                case 'V': cols[count++] = V_index(x, y); break;
                case 'I': cols[count++] = I_index(x, y); break;
                default: break;
            }

            tok= strtok(NULL, " ");
        }

        emit_row(cols, count);
    }
}

int main(void)
{
    build_matrix();
    return 0;
}
