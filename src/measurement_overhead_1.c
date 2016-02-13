#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define NUM_LOOP 100
#define INN_LOOP 1000

int main() {
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    uint64_t start, end;
    int i, j;
    FILE* fp;
    fp = fopen("../data/measurement_1.csv", "w");

    for (i=0; i<NUM_LOOP; i++) {
        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

        // perform the actual operation
        for (j=0; j<INN_LOOP; j++) {
            // NO op
        }

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32) | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
        fprintf(fp, "%ld,%ld\n", start, end);
    }
    return 0;
}
