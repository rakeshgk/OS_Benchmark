input_file = "task_creation_1.csv"

with open(input_file, "r") as ifp:
    with open("difference.txt", "w") as ofp:
        for line in ifp:
            line = line.strip()
            start, end = map(int, line.split(","))
            difference = abs(end - start)
            ofp.write("%s\n" % difference)
