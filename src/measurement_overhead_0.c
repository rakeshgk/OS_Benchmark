#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define NUM_TRIALS 1000

void inline measured_function(volatile int *var) {
    (*var) = 1;
}

int main() {
    /*
        Report the overhead of using RDTSCP and the 4 MOV instructions
        This is the overhead involved in reading time
        This overhead will be subtracted from subsequent results
    */
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    uint64_t start, end;
    double difference;
    int i;
    FILE* fp;
    fp = fopen("../data/measurement_overhead_0.csv", "w");

    for (i=0; i<NUM_TRIALS; i++) {
        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

        // perform the actual operation
        // measured_function(&variable);

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32) | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
        difference = (double) (end - start);
        fprintf(fp, "%lf\n", difference);
    }
    fclose(fp);
    return 0;
}
