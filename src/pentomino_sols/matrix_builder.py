#!/usr/bin/env python3

import sys
import ast

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

H = 8
W = 8

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

ALL_ORIENTATIONS = {
    name: orientations(shape)
    for name, shape in pieces.items()
}

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
    cell_index = {cell: i for i, cell in enumerate(cells)}
    return cells, cell_index

def valid_placement(shape, top, left, HOLE):
    coords = []
    for r, c in shape:
        rr, cc = r + top, c + left
        if rr < 0 or rr >= H or cc < 0 or cc >= W:
            return None
        if (rr, cc) in HOLE:
            return None
        coords.append((rr, cc))
    return coords

def generate_rows(HOLE, cell_index):
    rows = []
    piece_names = sorted(pieces.keys())
    for p_idx, name in enumerate(piece_names):
        for orient in ALL_ORIENTATIONS[name]:
            for r in range(H):
                for c in range(W):
                    placement = valid_placement(orient, r, c, HOLE)
                    if placement is None:
                        continue
                    cols = [cell_index[cell] for cell in placement]
                    cols.append(len(cell_index) + p_idx)
                    rows.append(cols)
    return rows

def main():
    global H, W
    if len(sys.argv) >= 4:
        H = int(sys.argv[2])
        W = int(sys.argv[3])
    HOLE = read_holes()
    cells, cell_index = build_board(HOLE)
    rows = generate_rows(HOLE, cell_index)
    ncols = len(cell_index) + len(pieces)
    print(len(rows), ncols, ncols)
    for row in rows:
        print(*row)

if __name__ == "__main__":
    main()
