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

char *fname;
long long num_mb_read;
long long fsize;
int runtime;
FILE *resfd;
bool done;
int procs;
int readsize;

void timer(int);

int main(int argc, char **argv)
{
    struct stat sb;
    int fd, i, runs, offset;
    long long pos;
    char *nadata, *data;
    char resfile[100];

    fname = argv[1];
    fsize = strtoull( argv[2], NULL, 10);
    runtime = atoi(argv[3]);
    readsize = atoi(argv[4]);
    procs = atoi(argv[5]);
    runs = atoi(argv[6]);

    // Allocate read buffer of size readsize
    nadata = (char *)malloc(readsize + BLK_SIZE);
    data = (char *)((((long)nadata + BLK_SIZE - 1) / BLK_SIZE) * BLK_SIZE);

    // create a results file
    snprintf(resfile, 100, "/home/amit/acads/cse221/OS_Benchmark/data/file_read_contention_%d.txt", procs);
    resfd = fopen(resfile, "a");
    if (!resfd) {
        printf("Opening a results file failed \n");
    }

    // Loop over tuples of file_size and timeout values and
    // run the experiment
    // for(fsize = (long long) 512*KB; fsize < (long long) 10*GB; fsize *= 2) {
    for (i=0; i < runs; i++) {
        // initialize control variables
        pos = 0;
        num_mb_read = 0;
        done = false;

        // check if the file is present
        if (stat(fname, &sb) == -1) {
            printf("error: cannot find a file %s \n", fname);
            exit(-1);
        }
        // printf("found file %s \n", fname);

        // open the file for read
        // XXX: Use O_DIRECT for direct disk access
        fd = open(fname, O_RDONLY | O_LARGEFILE);
        if (fd < 0) {
            printf("error: cannot open a file %s \n", fname);
            exit(-1);
        }
        // printf("opened file %s \n", fname);

        // Perform experiment
        // printf("Reading file of size %lld for %d seconds \n", fsize,  runtime);

        // Set timer
        signal(SIGALRM, &timer);
        alarm(runtime);
        // printf("alarm set \n");

        while (!done) {
            // Read readsize worth of data from a file
            offset = read(fd, data, readsize);
            if (offset < 0) {
                printf("error: could not read from file %s \n", fname);
                exit(-1);
            }
            // printf("read %d bytes from file %s \n", ret, fname);

            pos += offset;
            if (pos >= fsize) {
                if (lseek64(fd, 0, SEEK_SET) < 0) {
                    printf("error: could not seek to position 0 in file %s \n", fname);
                    exit(-1);
                }
                // printf("seek to 0 in file %s \n", fname);
                pos = 0;
                continue;
            }
            num_mb_read++;
            //printf("num_mb_read updated: %lld \n", num_mb_read);
        }
        close(fd);
    }
    fclose(resfd);
    free(nadata);

	return 0;
}

// Timer handler
void timer(int sig)
{
    // record results
    done = true;
    printf("Time is Up! %s %lld %lld %d \n", fname, fsize, num_mb_read, runtime);
    fprintf(resfd, "%lld,%d,%lld,%d,%d\n", fsize, readsize, num_mb_read, runtime, procs);
}
