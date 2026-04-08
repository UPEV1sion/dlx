BOARD_SIZE = 8
HOLE = {(3,3), (3,4), (4,3), (4,4)}

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

for i, sol in enumerate(solutions):
    write_ppm(sol, f"solution_{i+1}.ppm")
