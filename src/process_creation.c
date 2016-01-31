#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<sys/types.h>
#include<pthread.h>

#define OUTER_LOOP 10


int main(){
	int i=0;
	pid_t child_pid,tpid;
        int child_status;
	uint32_t cycles_high, cycles_low, cycles_high1, cycles_low1;
	uint64_t start, end, difference;
	for(i=0;i<OUTER_LOOP;i++){
		asm volatile ("cpuid\n\t"
			      "rdtsc\n\t"
			      "mov %%edx, %0\n\t"
			      "mov %%eax, %1\n\t"
			      : "=r" (cycles_high), "=r" (cycles_low)
				:: "%rax", "%rbx", "%rcx", "%rdx");
                child_pid=fork();
                if(child_pid == 0) {
                	/* This is done by the child process. */
                        exit(0);
                }
		else {
		/* This is run by the parent.  Wait for the child to terminate. */
			do {
				tpid = wait(&child_status);
			} while(tpid != child_pid);
		}
		asm volatile ("rdtscp\n\t"
			      "mov %%edx, %0\n\t"
			      "mov %%eax, %1\n\t"
			      "cpuid\n\t"
			      : "=r" (cycles_high1), "=r" (cycles_low1)
				:: "%rax", "%rbx", "%rcx", "%rdx");
	}

	start = (((uint64_t)cycles_high << 32)  | cycles_low);
	end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);

	difference = (end - start);

	printf("The difference is %lu - ", difference);
}
