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
    /*
        Need to measure the time involved in both creating
        and running a process

        Force flush the data to disk every time so that the
        output does not get buffered. Use the function fflush
    */
    uint64_t start, end;
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    int i;
    int child_status;
    pid_t child_pid, tpid;
    FILE* fp;
    fp = fopen("../data/task_creation_0.csv", "a");

    asm volatile (
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
        "%rax", "%rbx", "%rcx", "%rdx");

    // perform the actual operation
    child_pid = fork();

    asm volatile(
        "RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
        "%rbx", "%rcx", "%rdx");

    if(child_pid == 0) {
        exit(0);
    }
    else {
        /* This is run by the parent.  Wait for the child to terminate. */
        waitpid(child_pid, &child_status, 0);
    }

    start = (((uint64_t)cycles_high << 32) | cycles_low );
    end = (((uint64_t)cycles_high1 << 32) | cycles_low1 );
    fprintf(fp, "%lu,%lu\n", start, end);
    fclose(fp);
    return 0;
}
