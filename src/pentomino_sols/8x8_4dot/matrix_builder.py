pieces = {
    "F" : [(0,1), (0,2), (1,0), (1,1), (2,1)], 
    "I" : [(0,0), (1,0), (2,0), (3,0), (4,0)],
    "L" : [(0,0), (1,0), (2,0), (3,0), (3,1)],
    "N" : [(0,1), (1,1), (1,0), (2,0), (3,0)],
    "P" : [(0,0), (0,1), (1,0), (1,1), (2,0)],
    "T" : [(0,0), (0,1), (0,2), (1,1), (2,1)],
    "U" : [(0,0), (0,2), (1,0), (1,1), (1,2)],
    "V" : [(0,0), (1,0), (2,0), (2,1), (2,2)],
    "W" : [(0,0), (1,0), (1,1), (2,1), (2,2)],
    "X" : [(0,1), (1,0), (1,1), (1,2), (2,1)],
    "Y" : [(0,1), (1,0), (1,1), (2,1), (3,1)],
    "Z" : [(0,0), (0,1), (1,1), (2,1), (2,2)],
}

BOARD_SIZE = 8
HOLE = {(2,2), (2,5), (5,2), (5,5)}

cells = [(r, c) for r in range(BOARD_SIZE)
                for c in range(BOARD_SIZE)
                if (r, c) not in HOLE]

cell_index = {cell: i for i, cell in enumerate(cells)}

def rotate(shape):
    return [(-c, r) for r, c in shape]

def normalize(shape):
    min_r = min(r for r, c in shape)
    min_c = min(c for r, c in shape)
    return [(r - min_r, c - min_c) for r, c in shape]

def reflect(shape):
    return [(r, -c) for r, c in shape]

def orientations(shape):
    seen = set()

    for base in (shape, reflect(shape)):
        cur = base
        for _ in range(4):
            norm = tuple(sorted(normalize(cur)))
            seen.add(norm)
            cur = rotate(cur)

    return [list(o) for o in seen]

def valid_placement(shape, top, left):
    coords = []
    for r, c in shape:
        rr, cc = r + top, c + left
        if rr < 0 or rr >= BOARD_SIZE or cc < 0 or cc >= BOARD_SIZE:
            return None
        if (rr, cc) in HOLE:
            return None
        coords.append((rr, cc))
    return coords

def generate_rows():
    rows = []
    piece_names = list(pieces.keys())

    for p_idx, (name, shape) in enumerate(pieces.items()):
        for orient in orientations(shape):
            for r in range(BOARD_SIZE):
                for c in range(BOARD_SIZE):
                    placement = valid_placement(orient, r, c)
                    if placement is None:
                        continue

                    cols = []

                    for cell in placement:
                        cols.append(cell_index[cell])

                    cols.append(60 + p_idx)

                    rows.append(cols)

    return rows

rows = generate_rows()

print(len(rows), 72, len(pieces))
for row in rows:
    print(*row)
