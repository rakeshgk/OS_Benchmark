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
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

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

    //raw_local_irq_restore(flags);     /*we enable hard interrupts on our CPU*/
    //preempt_enable(); /*we enable preemption*/

    ret_val++;

    start = ( ((uint64_t)cycles_high << 32) | cycles_low );
    end =  ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );

    printf("Time: %lu\n", (end - start));

    return 0;
}
