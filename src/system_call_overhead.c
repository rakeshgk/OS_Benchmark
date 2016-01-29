#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#define OUTER_LOOP 10

// We can't add in-line functions in C without prefixing
// the function definition with either extern or inline
extern inline void inlineCallGetTimeOfDay(){
    struct timeval get_time;
    gettimeofday(&get_time, NULL);
}

void callGetTimeOfDay(){
    struct timeval get_time;
    gettimeofday(&get_time, NULL);
}

int main(){
    int i = 0;
    uint32_t cycles_high, cycles_low, cycles_high1, cycles_low1;
    uint64_t start, end, difference;

    for(i=0; i<OUTER_LOOP; ++i){
        asm volatile ("cpuid\n\t"
                      "rdtsc\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t"
                      : "=r" (cycles_high), "=r" (cycles_low)
                        :: "%rax", "%rbx", "%rcx", "%rdx");

        inlineCallGetTimeOfDay();
        //callGetTimeOfDay();

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
