#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    uint64_t start, end;
    pid_t process_pid;

    FILE* fp;
    fp = fopen("../data/system_call_overhead_0.csv", "a");

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
   
    fclose(fp);
    return 0;
}
