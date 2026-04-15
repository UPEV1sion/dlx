N = 8

ncols = 2*N
print(N * N, ncols, 0)

for i in range(N):
    for j in range(N):
        print(i, N + j)
