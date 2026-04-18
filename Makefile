CC := gcc
CFLAGS := -Wall -Wextra -pedantic -O3 -march=native

SRC := src
OUT := out
TETRASTICK_SOLVERS := $(SRC)/tetrasticks_sols

TETRASTICK_BUILDER := $(OUT)/tetrasticks/tetra_builder
TETRASTICK_PRINTER := $(OUT)/tetrasticks/tetra_printer
DLX := src/dlx

$(DLX): src/dlx.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(TETRASTICK_BUILDER): $(TETRASTICK_SOLVERS)/matrix_builder.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(TETRASTICK_PRINTER): $(TETRASTICK_SOLVERS)/solution_printer.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

tetrastick: $(TETRASTICK_BUILDER) $(TETRASTICK_PRINTER) $(DLX)
	$(TETRASTICK_BUILDER) | $(DLX) | $(TETRASTICK_PRINTER) $(OUT)/tetrasticks
	# $(TETRASTICK_BUILDER) | $(DLX) | $(TETRASTICK_PRINTER)

.PHONY: tetrastick 
