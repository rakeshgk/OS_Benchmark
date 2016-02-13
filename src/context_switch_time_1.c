#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>

uint64_t start, end;
unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
int fd[2];
FILE* fp;
    	

void *func(void *threadid) {
	read(fd[0], &start, sizeof(start));
	asm volatile ("rdtscp\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          "cpuid\n\t"
                          : "=r" (cycles_high1), "=r" (cycles_low1)
                            :: "%rax", "%rbx", "%rcx", "%rdx");

        end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
	fprintf(fp, "%ld,%ld\n", start, end);
	close(fd[0]);
	close(fd[1]);
	pthread_exit(NULL);
}

int main() {
	
	long i;
	pthread_t t;
	int rc;
	fp = fopen("../data/context_switch_time_1.csv", "w");
	pipe(fd);
	rc = pthread_create(&t, NULL, func, (void *)i);
	asm volatile ("cpuid\n\t"
                          "rdtsc\n\t"
                          "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                          : "=r" (cycles_high), "=r" (cycles_low)
                            :: "%rax", "%rbx", "%rcx", "%rdx");
       	start = ( ((uint64_t)cycles_high << 32) | cycles_low );

	write(fd[1], &start, sizeof(start));

	pthread_exit(NULL);
}
