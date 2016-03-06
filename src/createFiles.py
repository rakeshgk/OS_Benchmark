def main():
    fileSizes = [524288]
    size = 524288*2
    # while (size <= 8589934592):
    while (size <= 16777216):
        fileSizes.append(size)
        size = size*2

    for size in fileSizes:
        filename = ""
        if size < 1024*1024:
            filename = str(size/1024)+"KB"
        elif size < 1024*1024*1024:
            filename = str(size/(1024*1024))+"MB"
        else:
            filename = str(size/(1024*1024*1024))+"GB"
        f = open('../data/files/' + str(filename), "wb")
        f.seek(size-1)
        f.write("\0")
        f.close()

if __name__ == '__main__':
    main()
