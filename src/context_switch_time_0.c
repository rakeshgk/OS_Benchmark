#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define OUTER_LOOP 10

int main(){
    int i = 0;
    int childReturnStatus;
    uint32_t cycles_high = 0, cycles_low = 0, cycles_high1 = 0, cycles_low1 = 0;
    uint64_t start = 0, end = 0, difference = 0;
    pid_t process_pid;

    // Variables for setting up the pipe read and write
    char string[1] = "";
    char readbuffer[1];
    int nbytes;

    // To be used for creating pipes
    // fd[0] is set up for reading and fd[1] for writing
    int fd[2];

    for(i=0; i<OUTER_LOOP; ++i){
        // Setup the pipe
        pipe(fd);

        // Perform the fork
        process_pid = fork();

        // The child writes to the shared memory
        if(process_pid == 0){
            close(fd[0]);

            // Send a "string" through the output side of the pipe
            write(fd[1], string, (strlen(string) + 1));

            // Take the start time reading
            asm volatile ("cpuid\n\t"
                          "rdtsc\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          : "=r" (cycles_high), "=r" (cycles_low)
                            :: "%rax", "%rbx", "%rcx", "%rdx");

            start   = (((uint64_t)cycles_high << 32) | cycles_low);
            printf("Run - %d - The start value is %lu\n", i, start);
            exit(0);
        }
        // The parent receives a string from the shared memory
        else{
            close(fd[1]);

            // Wait for the child to complete
            waitpid(process_pid, &childReturnStatus, 0);

            // Read in a "string" from the pipe
            nbytes = read(fd[0], readbuffer, sizeof(readbuffer));

            // Take the end time reading
            asm volatile ("rdtscp\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          "cpuid\n\t"
                          : "=r" (cycles_high1), "=r" (cycles_low1)
                            :: "%rax", "%rbx", "%rcx", "%rdx");

            end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
            printf("Run - %d - The end value is %lu\n", i, end);
        }
    }
    return 0;
}
