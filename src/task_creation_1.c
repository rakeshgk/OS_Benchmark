#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define NUM_LOOP 1000

void pthread_function(void *ptr) {
    // Don't run anything inside this function
}

int main() {
    /*
        Measure the time taken to create and run a task using
        kernel threads
    */
    uint64_t start, end, total_clocks = 0;
    uint64_t avg_clock;
    uint32_t cycles_low, cycles_high, cycles_low1, cycles_high1;
    int i;
    FILE* fp;
    fp = fopen("../data/task_creation_1.csv", "a");

    pthread_t thread;

    asm volatile (
       "CPUID\n\t"
       "RDTSC\n\t"
       "mov %%edx, %0\n\t"
       "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
       "%rax", "%rbx", "%rcx", "%rdx");

    // perform the actual operation
    pthread_create(&thread, NULL, (void *)&pthread_function, NULL);

    asm volatile(
       "RDTSCP\n\t"
       "mov %%edx, %0\n\t"
       "mov %%eax, %1\n\t"
       "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
       "%rbx", "%rcx", "%rdx");


    start = (((uint64_t)cycles_high << 32) | cycles_low);
    end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
    fprintf(fp, "%lu,%lu\n", start, end);

    pthread_join(thread, NULL);
    fclose(fp);
    return 0;
}
