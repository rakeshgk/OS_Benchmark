#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)
#define BLK_SIZE 4096

int main(int argc, char **argv)
{
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    uint64_t start, end;
    struct stat sb;
    int fd, i, runs, offset, readsize;
    char *nadata, *data, *fname;
    unsigned long long num_read, fsize, max_reads;
    FILE *resfd;

    fname = argv[1];
    fsize = strtoull( argv[2], NULL, 10);
    readsize = atoi(argv[3]);
    runs = atoi(argv[4]);

    max_reads = fsize/readsize;
    num_read = 0;

    // Allocate read buffer of size readsize
    nadata = (char *)malloc(readsize + BLK_SIZE);
    data = (char *)((((long)nadata + BLK_SIZE - 1) / BLK_SIZE) * BLK_SIZE);

    // create a results file
    resfd = fopen("/home/amit/acads/cse221/OS_Benchmark/data/file_read_seq.csv", "a");
    if (!resfd) {
        printf("Opening a results file failed \n");
    }

    // check if the file is present
    if (stat(fname, &sb) == -1) {
        printf("error: cannot find a file %s \n", fname);
        exit(-1);
    }
    // printf("found file %s \n", fname);

    // open the file for read
    // XXX: Use O_DIRECT for direct disk access
    fd = open(fname, O_RDONLY | O_LARGEFILE | O_DIRECT);
    if (fd < 0) {
        printf("error: cannot open a file %s \n", fname);
        exit(-1);
    }
    // printf("opened file %s \n", fname);

    // Warmup so that the file contents are in buffer cache
    while (num_read < max_reads) {
        // Read readsize worth of data from a file
        offset = read(fd, data, readsize);
        if (offset < 0) {
            printf("error: could not read from file %s \n", fname);
            exit(-1);
        }
        num_read++;
    }

    // run the experiment
    for (i=0; i < runs; i++) {
        // initialize control variables
        num_read = 0;

        // seek to the beginning
        if (lseek64(fd, 0, SEEK_SET) < 0) {
            printf("error: could not seek to position 0 in file %s \n", fname);
            exit(-1);
        }

        // start time
        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

        while (num_read < max_reads) {
            // Read readsize worth of data from a file
            offset = read(fd, data, readsize);
            if (offset < 0) {
                printf("error: could not read from file %s \n", fname);
                exit(-1);
            }
            num_read++;
            //printf("num_mb_read updated: %llu \n", num_mb_read);
        }

        //end time
        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32) | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
        printf("Time is Up! %s %llu %llu %lu \n", fname, fsize, num_read, end-start);
        fprintf(resfd, "%llu,%d,%llu,%lu \n", fsize, readsize, num_read, end-start);
    }
    close(fd);
    fprintf(resfd, "\n");
    fclose(resfd);
    free(nadata);

	return 0;
}
