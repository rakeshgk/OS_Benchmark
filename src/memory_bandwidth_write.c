#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_LOOP 1000
#define CACHE_SIZE 4736000
#define offset 1184000
#define MEM_SIZE 8000000

int main() {
    uint64_t start, end;
    int i,j,temp,counter;
    int *array;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
    FILE* fp;
    fp = fopen("../data/memory_bandwidth_write.csv", "w");
    array=(int *)malloc(MEM_SIZE);
    counter=MEM_SIZE/sizeof(int);
    for (i=0; i<NUM_LOOP; i++) {
	asm volatile ("cpuid\n\t"
		          "rdtsc\n\t"
		          "mov %%edx, %0\n\t"
		          "mov %%eax, %1\n\t"
		          : "=r" (cycles_high), "=r" (cycles_low)
		           :: "%rax", "%rbx", "%rcx", "%rdx");
	start = ( ((uint64_t)cycles_high << 32) | cycles_low );
	for(j=0;j<counter;j++){
		array[j]=0;
	}

	asm volatile ("rdtscp\n\t"
		          "mov %%edx, %0\n\t"
		          "mov %%eax, %1\n\t"
		          "cpuid\n\t"
		          : "=r" (cycles_high1), "=r" (cycles_low1)
		            :: "%rax", "%rbx", "%rcx", "%rdx");

	end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
	fprintf(fp, "%ld,%ld\n", start, end);
    }
    return 0;
}
