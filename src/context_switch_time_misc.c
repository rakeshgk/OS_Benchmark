#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
//#include <sys/wait.h>

#define NUM_LOOP 30

int main() {
    uint64_t start, end;
    int i;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
    int fd[2];
    FILE* fp;
    fp = fopen("../data/context_switch_time_misc.csv", "w");
    pipe(fd);
    for (i=0; i<NUM_LOOP; i++) {
    asm volatile ("cpuid\n\t"
                          "rdtsc\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          : "=r" (cycles_high), "=r" (cycles_low)
                            :: "%rax", "%rbx", "%rcx", "%rdx");
    start = ( ((uint64_t)cycles_high << 32) | cycles_low );
    
    write(fd[1], &start, sizeof(start));
    read(fd[0], &start, sizeof(start));

    asm volatile ("rdtscp\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          "cpuid\n\t"
                          : "=r" (cycles_high1), "=r" (cycles_low1)
                          :: "%rax", "%rbx", "%rcx", "%rdx");

    end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );

    fprintf(fp, "%lu,%lu\n", start, end);
    }
  
    return 0;
}
