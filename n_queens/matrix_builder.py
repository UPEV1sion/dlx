N = 4

ncols = 6*N - 2
print(N * N, ncols, abs(ncols - N*N))

for i in range(N):
    for j in range(N):
        row = i
        col = N + j
        diag1 = 2*N + (i - j + (N - 1))
        diag2 = 4*N - 1 + (i + j)

        print(row, col, diag1, diag2)
