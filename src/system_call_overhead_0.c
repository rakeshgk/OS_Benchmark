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
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    uint64_t start, end;
    int i;
    pid_t child_pid, process_pid;
    int child_status;

    FILE* fp;
    fp = fopen("../data/system_call_overhead_0.csv", "w");

    for (i=0; i<NUM_LOOP; i++) {
        child_pid = fork();
        if(child_pid == 0) {
            /*
                We take the PID value from within the child process.
                This is done to ensure that the PID values are not cached
            */
            asm volatile (
                "CPUID\n\t"
                "RDTSC\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
                "%rax", "%rbx", "%rcx", "%rdx");

            process_pid = getpid();

            asm volatile(
                "RDTSCP\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
                "%rbx", "%rcx", "%rdx");

            start = (((uint64_t)cycles_high << 32) | cycles_low);
            end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
            fprintf(fp, "%lu,%lu\n", start, end);
            exit(0);
        }
        else {
            /* This is run by the parent.
               The parent does nothing apart from waiting for the child
            */
            waitpid(child_pid, &child_status, 0);
        }
    }
    fclose(fp);
    return 0;
}
