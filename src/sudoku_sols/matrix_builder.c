#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#define NROWS 545
#define NCOLS 324

#define BOARD_DIM 9
#define NUM_CELLS (BOARD_DIM * BOARD_DIM)

#define GET_R(c) (c / BOARD_DIM)
#define GET_C(c) (c % BOARD_DIM)
#define GET_B(c) ((GET_R(c) / 3) * 3 + (GET_C(c) / 3))

#define MAP_ROW(n, c) (NUM_CELLS * 1 + (GET_R(c) * 9 + n))
#define MAP_COL(n, c) (NUM_CELLS * 2 + (GET_C(c) * 9 + n))
#define MAP_BLK(n, c) (NUM_CELLS * 3 + (GET_B(c) * 9 + n))

void print_matrix_row(uint8_t num, size_t cell)
{
    printf("%zu %ld %ld %ld\n", cell, MAP_ROW(num, cell), MAP_COL(num, cell), MAP_BLK(num, cell));
}

void build_matrix(uint8_t *board)
{
    printf("%d %d %d\n", (int) NROWS, (int) NCOLS, (int) NCOLS);

    for(size_t cell = 0; cell < NUM_CELLS; ++cell)
    {
        if(board[cell] > 0)
        {
            print_matrix_row(board[cell] - 1, cell);
        }
        else
        {
            for(uint8_t num = 1; num <= 9; ++num)
            {
                print_matrix_row(num - 1, cell);        
            }
        }
    }
}

int main(int argc, char **argv)
{
    if(argc <= 1)
    {
        fprintf(stderr, "USAGE: %s <matrix>\n", argv[0]);
        return 1;
    }

    uint8_t board[NUM_CELLS];
    size_t board_idx = 0;

    FILE *f = fopen(argv[1], "r");
    assert(f);

    char buffer[1024];
    while(fgets(buffer, sizeof buffer, f) != NULL)
    {
        char *tok = strtok(buffer, " \n"); 
        while(tok)
        {
            char *endptr;
            const long cell = strtol(tok, &endptr, 10);
            if(*endptr != 0)
            {
                fprintf(stderr, "ERROR: invalid number %s\n", tok);
                return 1;
            }

            if(cell > 9 || cell < 0)
            {
                fprintf(stderr, "ERROR: number out of range 0-9: %ld\n", cell);
                return 1;
            }
            
            board[board_idx++] = (uint8_t) cell;

            tok = strtok(NULL, " \n");
        }
    }
    
    if(board_idx != NUM_CELLS)
    {
        fprintf(stderr, "ERROR: invalid matrix dimentions. Matrix is not 9x9!\n");
        return 1;
    }

    build_matrix(board);

    fclose(f);

    return 0;
}
