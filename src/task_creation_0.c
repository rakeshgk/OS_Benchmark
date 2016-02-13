#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_LOOP 1000

int main() {
    uint64_t start, end;
    // total_clocks = 0, avg_clock;

    int i;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
    pid_t child_pid,tpid;
    int child_status;

    for (i=0; i<NUM_LOOP; i++) {
        //printf("Iteration # %d \n", i);

        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

        // perform the actual operation
        child_pid = fork();
        if(child_pid == 0) {
            /* This is done by the child process. */
            //printf("Executing child, Iteration # %d \n", i);
            asm volatile(
                "RDTSCP\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                "%rbx", "%rcx", "%rdx");
            end = (double) ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
            printf("Run: %d, End Time: %lu \n", i, end );
            exit(0);
        } else {
            /* This is run by the parent.  Wait for the child to terminate. */
            //printf("Executing parent, Iteration # %d \n", i);
            do {
                tpid = wait(&child_status);
            } while(tpid != child_pid);
        }

        start = (double) ( ((uint64_t)cycles_high << 32) | cycles_low );
        // = (double) ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );

        printf("Run: %d, Start Time: %lu \n", i, start );
        //total_clocks += (end - start);
    }
    //avg_clock = total_clocks/NUM_LOOP;
    //printf("Avg Time: %lu \n", avg_clock);
    return 0;
}
