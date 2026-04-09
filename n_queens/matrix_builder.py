#!/usr/bin/env python3
import sys

if len(sys.argv) != 2:
    print("Usage: python3 parse_dlx.py N")
    sys.exit(1)
N = int(sys.argv[1])

ncols = 6*N - 2
print(N * N, ncols, 2*N)

for i in range(N):
    for j in range(N):
        row = i
        col = N + j
        diag1 = 2*N + (i - j + (N - 1))
        diag2 = 4*N - 1 + (i + j)

        print(row, col, diag1, diag2)
