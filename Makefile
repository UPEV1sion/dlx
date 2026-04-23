CC := gcc
CFLAGS := -Wall -Wextra -pedantic -O3 -march=native

SRC := src
OUT := out

PENTOMINO_SOLVERS := $(SRC)/pentomino_sols
TETRASTICK_SOLVERS := $(SRC)/tetrasticks_sols
N_QUEEN_SOLVERS := $(SRC)/n_queens_sols
SUDOKU_SOLVER := $(SRC)/sudoku_sols
HEXIAMOND_SOLVER := $(SRC)/hexiamond_sols

PENTOMINO_BUILDER := $(PENTOMINO_SOLVERS)/matrix_builder.py
PENTOMINO_PRINTER := $(PENTOMINO_SOLVERS)/solution_printer.py

N_QUEENS_BUILDER := $(N_QUEEN_SOLVERS)/matrix_builder.py
N_QUEENS_PRINTER := $(N_QUEEN_SOLVERS)/solution_printer.py

TETRASTICK_BUILDER := $(OUT)/tetrasticks/tetra_builder
TETRASTICK_PRINTER := $(OUT)/tetrasticks/tetra_printer

SUDOKU_BUILDER := $(OUT)/sudoku/sudoku_builder
SUDOKU_PRINTER := $(OUT)/sudoku/sudoku_printer

HEXIAMOND_BUILDER := $(OUT)/hexiamond/matrix_builder

DLX := out/dlx

$(DLX): src/dlx.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(TETRASTICK_BUILDER): $(TETRASTICK_SOLVERS)/matrix_builder.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(TETRASTICK_PRINTER): $(TETRASTICK_SOLVERS)/solution_printer.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(SUDOKU_BUILDER): $(SUDOKU_SOLVER)/matrix_builder.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(SUDOKU_PRINTER): $(SUDOKU_SOLVER)/solution_printer.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(HEXIAMOND_BUILDER): $(HEXIAMOND_SOLVER)/matrix_builder.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

tetrastick: $(TETRASTICK_BUILDER) $(TETRASTICK_PRINTER) $(DLX)
	$(TETRASTICK_BUILDER) | $(DLX) | $(TETRASTICK_PRINTER) $(OUT)/tetrasticks

pentomino: $(PENTOMINO_BUILDER) $(PENTOMINO_PRINTER) $(DLX)
	mkdir -p out/pentomino
	cd out/pentomino && ../../$(PENTOMINO_BUILDER) | ../../$(DLX) | ../../$(PENTOMINO_PRINTER)

n_queen: $(N_QUEENS_BUILDER) $(N_QUEENS_PRINTER) $(DLX)
	$(N_QUEENS_BUILDER) | $(DLX) | $(N_QUEENS_PRINTER)

sudoku: $(SUDOKU_BUILDER) $(SUDOKU_PRINTER) $(DLX)
	$(SUDOKU_BUILDER) $(SUDOKU_SOLVER)/hardest_sudoku.txt | $(DLX) | $(SUDOKU_PRINTER)

.PHONY: tetrastick pentomino n_queen sudoku
