BOARD_SIZE = 8
HOLE = {(2,2), (2,5), (5,2), (5,5)}

cells = [(r, c) for r in range(BOARD_SIZE)
                for c in range(BOARD_SIZE)
                if (r, c) not in HOLE]
index_to_cell = {i: cell for i, cell in enumerate(cells)}

pieces = ["F","I","L","N","P","T","U","V","W","X","Y","Z"]

colors = [
    (255,0,0), (0,255,0), (255,255,0), (0,0,255),
    (255,0,255), (0,255,255), (255,128,128),(128,255,128),
    (255,255,128),(128,128,255),(255,128,255),(128,255,255)
]

def parse_solutions(path):
    solutions = []
    current = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line:
                if current:
                    solutions.append(current)
                    current = []
                continue
            nums = list(map(int, line.split()))
            current.append(nums)
    if current:
        solutions.append(current)
    return solutions

def solution_to_grid(solution):
    grid = [[None]*BOARD_SIZE for _ in range(BOARD_SIZE)]
    for row in solution:
        piece_id = next(x for x in row if x >= 60) - 60
        for x in row:
            if x < 60:
                r, c = index_to_cell[x]
                grid[r][c] = piece_id
    return grid

import copy

def rotate_grid(grid):
    """Rotate 90 degrees clockwise"""
    return [list(row) for row in zip(*grid[::-1])]

def reflect_grid(grid):
    """Reflect horizontally"""
    return [row[::-1] for row in grid]

def all_symmetries(grid):
    """Return all 8 symmetric variants (4 rotations × 2 reflections)"""
    grids = []
    g = copy.deepcopy(grid)
    for _ in range(4):
        grids.append(g)
        grids.append(reflect_grid(g))
        g = rotate_grid(g)
    return grids

def grid_to_tuple(grid):
    """Convert grid to a hashable tuple for duplicate detection"""
    return tuple(tuple(row) for row in grid)

def write_ppm(solution, filename, scale=20):
    board = [[(0,0,0) for _ in range(BOARD_SIZE)] for _ in range(BOARD_SIZE)]
    for r,c in HOLE:
        board[r][c] = (128,128,128)
    for row in solution:
        piece_col = next(x for x in row if x >= 60)
        color = colors[piece_col-60]
        for x in row:
            if x < 60:
                r,c = index_to_cell[x]
                board[r][c] = color
    w = BOARD_SIZE * scale
    h = BOARD_SIZE * scale
    with open(filename, "w") as f:
        f.write(f"P3\n{w} {h}\n255\n")
        for r in range(BOARD_SIZE):
            for _ in range(scale):
                for c in range(BOARD_SIZE):
                    for _ in range(scale):
                        f.write(f"{board[r][c][0]} {board[r][c][1]} {board[r][c][2]} ")
                f.write("\n")

solutions = parse_solutions("matrix.out")

seen = set()
unique_solutions = []

for sol in solutions:
    grid = solution_to_grid(sol)
    variants = all_symmetries(grid)
    hashes = [grid_to_tuple(v) for v in variants]
    canonical = min(hashes)
    if canonical not in seen:
        seen.add(canonical)
        unique_solutions.append(sol)

print(f"{len(unique_solutions)} unique solutions out of {len(solutions)} total")

for i, sol in enumerate(unique_solutions):
    write_ppm(sol, f"solution_{i+1}.ppm")
