#!/usr/bin/env python3

import sys
import ast
import copy

H = 8
W = 8

colors = [
    (255,0,0), (0,255,0), (255,255,0), (0,0,255),
    (255,0,255), (0,255,255), (255,128,128),(128,255,128),
    (255,255,128),(128,128,255),(255,128,255),(128,255,255)
]

def read_holes():
    if len(sys.argv) >= 2:
        with open(sys.argv[1]) as f:
            data = f.read().strip()
        try:
            return set(ast.literal_eval(data))
        except:
            return {ast.literal_eval(x) for x in data.split()}
    else:
        print("No holes provided, defaulting to (3,3) (3,4) (4,3) (4,4).", file=sys.stderr)
        return {(3,3), (3,4), (4,3), (4,4)}

def build_board(HOLE):
    cells = [(r, c) for r in range(H)
                    for c in range(W)
                    if (r, c) not in HOLE]
    index_to_cell = {i: cell for i, cell in enumerate(cells)}
    return cells, index_to_cell

def parse_solutions():
    solutions = []
    current = []
    for line in sys.stdin:
        line = line.strip()
        if not line:
            if current:
                solutions.append(current)
                current = []
            continue
        current.append(list(map(int, line.split())))
    if current:
        solutions.append(current)
    return solutions

def solution_to_grid(solution, index_to_cell):
    grid = [[None]*W for _ in range(H)]
    for row in solution:
        piece_id = next(x for x in row if x >= 60) - 60
        for x in row:
            if x < 60:
                r, c = index_to_cell[x]
                grid[r][c] = piece_id
    return grid

def rotate_grid(grid):
    return [list(row) for row in zip(*grid[::-1])]

def reflect_grid(grid):
    return [row[::-1] for row in grid]

def all_symmetries(grid):
    grids = []
    g = copy.deepcopy(grid)
    for _ in range(4):
        grids.append(g)
        grids.append(reflect_grid(g))
        g = rotate_grid(g)
    return grids

def grid_to_tuple(grid):
    return tuple(tuple(row) for row in grid)

def write_ppm(solution, index_to_cell, HOLE, filename, scale=20):
    board = [[(0,0,0) for _ in range(W)] for _ in range(H)]
    for r, c in HOLE:
        board[r][c] = (128,128,128)
    for row in solution:
        piece_id = next(x for x in row if x >= 60) - 60
        color = colors[piece_id]
        for x in row:
            if x < 60:
                r, c = index_to_cell[x]
                board[r][c] = color
    with open(filename, "w") as f:
        f.write(f"P3\n{W*scale} {H*scale}\n255\n")
        for r in range(H):
            for _ in range(scale):
                for c in range(W):
                    for _ in range(scale):
                        f.write(f"{board[r][c][0]} {board[r][c][1]} {board[r][c][2]} ")
                f.write("\n")

def main():
    global H, W
    if len(sys.argv) >= 4:
        H = int(sys.argv[2])
        W = int(sys.argv[3])
    HOLE = read_holes()
    cells, index_to_cell = build_board(HOLE)
    solutions = parse_solutions()
    seen = set()
    unique = []
    for sol in solutions:
        grid = solution_to_grid(sol, index_to_cell)
        variants = all_symmetries(grid)
        canonical = min(grid_to_tuple(v) for v in variants)
        if canonical not in seen:
            seen.add(canonical)
            unique.append(sol)
    print(f"{len(unique)} unique solutions out of {len(solutions)} total")
    for i, sol in enumerate(unique):
        write_ppm(sol, index_to_cell, HOLE, f"solution_{i+1}.ppm")

if __name__ == "__main__":
    main()
