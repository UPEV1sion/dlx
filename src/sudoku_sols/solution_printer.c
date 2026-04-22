#include <stdio.h>
#include <assert.h>

#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))
#define BOARD_DIM 9
#define NUM_CELLS (BOARD_DIM * BOARD_DIM)

int main(void)
{
    int board[81] = {0};
    int idx[4];
    size_t count = 0;

    while (scanf("%d %d %d %d", idx + 0, idx + 1, idx + 2, idx + 3) == 4)
    {
        int cell = -1;
        int val = -1;

        for (int i = 0; i < 4; i++)
        {
            if (idx[i] >= 0 && idx[i] <= NUM_CELLS - 1)
            {
                cell = idx[i];
            }
            if (idx[i] >= NUM_CELLS && idx[i] <= 2 * NUM_CELLS - 1)
            {
                val = ((idx[i] - 81) % 9) + 1;
            }
        }

        assert(cell != -1 && val != -1);

        board[cell] = val;
        count++;
    }

    if (count < NUM_CELLS)
    {
        fprintf(stderr, "ERROR: invalid solution. Only %zu cells were found.\n", count);
        return 1;
    }

    printf("\n+-------+-------+-------+\n");
    for (size_t r = 0; r < BOARD_DIM; r++)
    {
        printf("| ");
        for (size_t c = 0; c < BOARD_DIM; c++)
        {
            printf("%d ", board[r * BOARD_DIM + c]);
            if ((c + 1) % 3 == 0) printf("| ");
        }
        printf("\n");
        if ((r + 1) % 3 == 0)
        {
            printf("+-------+-------+-------+\n");
        }
    }

    return 0;
}
