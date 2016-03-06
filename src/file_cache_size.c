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

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

char *data;
char *fname;
unsigned long long num_mb_read;
unsigned long long fsize;
int runtime;
FILE *resfd;
bool done;

void get_finfo(unsigned long long, char *, int *, char *);
void timer(int);

int main(int argc, char **argv)
{
    struct stat sb;
    int fd, i, offset, readsize;
    unsigned long long pos;

    fname = argv[1];
    fsize = strtoull( argv[2], NULL, 10);
    runtime = atoi(argv[3]);
    readsize = atoi(argv[4]);

    // Allocate read buffer of size 1MB
    data = (char *) malloc(MB);

    // create a results file
    resfd = fopen("/home/amit/acads/cse221/OS_Benchmark/data/file_cache_results.txt", "a");
    if (!resfd) {
        printf("Opening a results file failed \n");
    }

    // Loop over tuples of file_size and timeout values and
    // run the experiment
    // for(fsize = (unsigned long long) 512*KB; fsize < (unsigned long long) 10*GB; fsize *= 2) {
    for (i=0; i < 2; i++) {
        // initialize control variables
        pos = 0;
        num_mb_read = 0;
        done = false;

        // check if the file is present
        if (stat(fname, &sb) == -1) {
            printf("error: cannot find a file %s \n", fname);
            exit(EXIT_SUCCESS);
        }
        // printf("found file %s \n", fname);

        // open the file for read
        fd = open(fname, O_RDONLY | O_LARGEFILE);
        if (fd < 0) {
            printf("error: cannot open a file %s \n", fname);
            exit(EXIT_SUCCESS);
        }
        // printf("opened file %s \n", fname);

        // Perform experiment
        // printf("Reading file of size %llu for %d seconds \n", fsize,  runtime);

        // Set timer
        signal(SIGALRM, &timer);
        alarm(runtime);
        // printf("alarm set \n");

        while (!done) {
            // Read 1MB worth of data from a file
            offset = read(fd, data, readsize);
            if (offset < 0) {
                printf("error: could not read from file %s \n", fname);
                exit(EXIT_SUCCESS);
            }
            // printf("read %d bytes from file %s \n", ret, fname);

            pos += offset;
            if (pos >= fsize) {
                offset = lseek64(fd, 0, SEEK_SET);
                if (offset == -1) {
                    printf("error: could not seek to position 0 in file %s \n", fname);
                    exit(EXIT_SUCCESS);
                }
                // printf("seek to 0 in file %s \n", fname);
                pos = 0;
                continue;
            }
            num_mb_read++;
            //printf("num_mb_read updated: %llu \n", num_mb_read);
        }
        close(fd);
    }
    fclose(resfd);
    free(data);

	return 0;
}

// Timer handler
void timer(int sig)
{
    // record results
    done = true;
    printf("Time is Up! %s %llu %llu %d \n", fname, fsize, num_mb_read, runtime);
    fprintf(resfd, "%llu %llu %d \n", fsize, num_mb_read, runtime);
}
