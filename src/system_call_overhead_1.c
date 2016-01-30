#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define OUTER_LOOP 10

uint32_t function_that_returns_uint32(){
    return 42;
}

int main(){
    int i = 0;
    uint32_t cycles_high, cycles_low, cycles_high1, cycles_low1;
    uint64_t start, end, difference;
    uint32_t ret_int;

    for(i=0; i<OUTER_LOOP; ++i){
        asm volatile ("cpuid\n\t"
                      "rdtsc\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t"
                      : "=r" (cycles_high), "=r" (cycles_low)
                        :: "%rax", "%rbx", "%rcx", "%rdx");

        ret_int = function_that_returns_uint32();

        asm volatile ("rdtscp\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t"
                      "cpuid\n\t"
                      : "=r" (cycles_high1), "=r" (cycles_low1)
                        :: "%rax", "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32)  | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);

        difference = (end - start);

        printf("The difference is %lu - ", difference);
    }
    return 0;
}
