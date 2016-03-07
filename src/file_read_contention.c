#define _GNU_SOURCE

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
#include <sys/wait.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)
#define BLK_SIZE 4096

char *fname;
char *resfile;
unsigned long long *num_mb_read = NULL;
unsigned long long fsize;
int runtime;
FILE *resfd;
bool *done = NULL;
pid_t *pids = NULL;
int procs;

void timer(int);
int get_my_i(pid_t);

int main(int argc, char **argv)
{
    struct stat sb;
    int fd, i, offset, readsize;
    unsigned long long pos;
    char **nadata = NULL, **data = NULL;
    char fpath[100];
    int status;
    pid_t pid;

    // extract command line arguments
    fname = argv[1];
    fsize = strtoull( argv[2], NULL, 10);
    runtime = atoi(argv[3]);
    readsize = atoi(argv[4]);
    procs = atoi(argv[5]);
    resfile = argv[6];

    // Allocate per proc data structures
    num_mb_read = (unsigned long long *) malloc(procs * sizeof(unsigned long long));
    done = (bool*)malloc(procs * sizeof(bool));
    data = (char **) malloc(procs * sizeof(char*));
    nadata = (char **) malloc(procs * sizeof(char*));
    pids = (pid_t *) malloc(procs * sizeof(pid_t));

    if (!num_mb_read || !done || !data || !nadata || !pids) {
        printf("Allocation Failure. \n");
    }

    // create a results file
    resfd = fopen(resfile, "a");
    if (!resfd) {
        printf("Opening a results file failed \n");
    }

    // Loop over tuples of file_size and timeout values and
    // run the experiment
    // for(fsize = (unsigned long long) 512*KB; fsize < (unsigned long long) 10*GB; fsize *= 2) {
    for (i=0; i < procs; i++) {
        // fork a child
        pids[i] = fork();
        if (pids[i] < 0) {
            // failure
            printf("error: creating child failed. Exiting! \n");
        } else if (pids[i] == 0) {
            // child code
            printf("success: created child %d. \n", i);
            // initialize control variables
            pos = 0;
            num_mb_read[i] = 0;
            done[i] = false;

            // Allocate read buffer of size readsize
            nadata[i] = (char *) malloc(readsize + BLK_SIZE);
            data[i] = (char *)((((long)nadata + BLK_SIZE - 1) / BLK_SIZE) * BLK_SIZE);

            if (!data[i] || !nadata[i]) {
                printf("Allocation Failure. \n");
            }

            snprintf(fpath, 100, "%s/%d", fname, i);
            strcat(fpath, "_1GB");

            // check if the file is present
            if (stat(fpath, &sb) == -1) {
                printf("error: cannot find a file %s \n", fpath);
                exit(-1);
            }
            // printf("found file %s \n", fname);

            // open the file for read
            fd = open(fpath, O_RDONLY | O_LARGEFILE | O_DIRECT);
            if (fd < 0) {
                printf("error: cannot open a file %s \n", fpath);
                exit(-1);
            }
            // printf("opened file %s \n", fname);

            // Perform experiment
            // printf("Reading file of size %llu for %d seconds \n", fsize,  runtime);

            // Set timer
            signal(SIGALRM, &timer);
            alarm(runtime);
            // printf("alarm set \n");

            while (!done[i]) {
                // Read readsize worth of data from a file
                offset = read(fd, data[i], readsize);
                if (offset < 0) {
                    printf("error: could not read from file %s \n", fpath);
                    exit(-1);
                }
                // printf("read %d bytes from file %s \n", ret, fname);

                pos += offset;
                if (pos >= fsize) {
                    offset = lseek64(fd, 0, SEEK_SET);
                    if (offset == -1) {
                        printf("error: could not seek to position 0 in file %s \n", fpath);
                        exit(-1);
                    }
                    // printf("seek to 0 in file %s \n", fname);
                    pos = 0;
                    continue;
                }
                num_mb_read[i]++;
                //printf("num_mb_read updated: %llu \n", num_mb_read);
            }
            close(fd);
            free(nadata[i]);
            exit(EXIT_SUCCESS);
        } else {
            // parent code
            printf("Created child %d \n", i);
        }
    }

    // lets wait for all children to finish
    while (i >= 0) {
        pid = wait(&status);
        printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
        i--;
    }

    // release resources
    fclose(resfd);
    free(nadata);
    free(data);
    free(num_mb_read);
    free(done);

	return 0;
}

// Timer handler
void timer(int sig)
{
    pid_t pid = getpid();
    int i = get_my_i(pid);
    if (i < 0) {
        printf("error: could not find i for pid: %d, something bad happened \n", pid);
        exit(-1);
    }
    // record results
    done[i] = true;
    printf("Time is Up! %d %llu %llu %d \n", i, fsize, num_mb_read[i], runtime);
    fprintf(resfd, "%d %llu %llu %d \n", i, fsize, num_mb_read[i], runtime);
}

int get_my_i(pid_t pid) {
    int i;
    for (i = 0; i < procs; i++) {
        if (pids[i] == pid)
            return i;
    }
    return -1;
}
