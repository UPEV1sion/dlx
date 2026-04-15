#!/usr/bin/env python3

import sys

def parse_solutions(lines, N):
    solutions = []
    current = []

    for line in lines:
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


def decode_solution(sol, N):
    positions = []

    for row_data in sol:
        r = None
        c = None

        for x in row_data:
            if 0 <= x < N:
                r = x
            elif N <= x < 2 * N:
                c = x - N

        if r is None or c is None:
            raise ValueError(f"Invalid row: {row_data}")

        positions.append((r, c))

    return positions


def print_board(positions, N):
    board = [['.' for _ in range(N)] for _ in range(N)]

    for r, c in positions:
        board[r][c] = 'Q'

    for row in board:
        print(' '.join(row))
    print()


def main():
    if len(sys.argv) == 2:
        N = int(sys.argv[1])
    else:
        N = 8

    lines = sys.stdin.readlines()
    solutions = parse_solutions(lines, N)

    for i, sol in enumerate(solutions, 1):
        print(f"Solution {i}:")
        positions = decode_solution(sol, N)
        print_board(positions, N)


if __name__ == "__main__":
    main()
