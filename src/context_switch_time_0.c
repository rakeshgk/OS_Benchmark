#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_LOOP 1000

int main() {
    uint64_t start, end;
    uint32_t cycles_low, cycles_high, cycles_low1, cycles_high1;
    int i;

    pid_t process_pid;
 
    // To be used for creating pipes
    // fd[0] is set up for reading and fd[1] for writing
    int fd[2];
    FILE* fp;
    fp = fopen("../data/context_switch_time_0.csv", "w");

    for (i=0; i<NUM_LOOP; i++) {

        // Setup the pipe
        pipe(fd);

        // Perform the fork
        process_pid = fork();

        // The child reads from the shared memory
        if(process_pid == 0){
            read(fd[0],&start,sizeof(start));
		
            // Take the end time reading
            asm volatile ("rdtscp\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          "cpuid\n\t"
                          : "=r" (cycles_high1), "=r" (cycles_low1)
                            :: "%rax", "%rbx", "%rcx", "%rdx");

            end = (((uint64_t)cycles_high1 << 32) | cycles_low1);
	        fprintf(fp, "%lu,%lu\n", start, end);
            fflush(fp); 
            return 0;
        }
        else {
	    // Take the start time reading
            asm volatile ("cpuid\n\t"
                          "rdtsc\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          : "=r" (cycles_high), "=r" (cycles_low)
                            :: "%rax", "%rbx", "%rcx", "%rdx");

            start = (((uint64_t)cycles_high << 32) | cycles_low);
	        write(fd[1],&start,sizeof(start));
        }
	close(fd[0]);
	close(fd[1]);
    }
    return 0;
}
