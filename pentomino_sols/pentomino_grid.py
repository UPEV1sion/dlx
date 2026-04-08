HOLE = {(3,3), (3,4), (4,3), (4,4)}
counter = 0
for i in range(8):
    for j in range(8):
        if (i,j) not in HOLE:
            print(f"{counter:03} ", end="")
            counter += 1
        else:
            print("    ", end="")
    print()
