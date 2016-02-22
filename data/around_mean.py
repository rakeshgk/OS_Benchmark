import statistics

numbers = []
around = 6300

with open("difference.txt", "r") as fp:
    for line in fp:
        line = int(line.strip())
        if line > around + 250:
            continue
        elif line < around - 250:
            continue
        else:
            numbers.append(line)

print("Working on data set size %s" % len(numbers))
print(statistics.mean(numbers))
print(statistics.pstdev(numbers))
