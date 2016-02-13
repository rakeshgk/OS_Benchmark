#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define NUM_LOOP 1000

uint32_t function_that_returns_uint32(){
    return 42;
}

int main() {
    uint64_t start, end;
    uint32_t ret_val;
    uint32_t cycles_low, cycles_high, cycles_low1, cycles_high1;
    FILE* fp;
    fp = fopen("../data/system_call_overhead_1.csv", "a");

    asm volatile (
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
        "%rax", "%rbx", "%rcx", "%rdx");

    // perform the actual operation
    ret_val = function_that_returns_uint32();

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
