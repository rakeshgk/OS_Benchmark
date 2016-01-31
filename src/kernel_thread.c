<<<<<<< HEAD
#include <unistd.h>     
#include <sys/types.h>   
#include <errno.h>      
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <string.h> 
#include<stdint.h>
#include<time.h>
#define OUTER_LOOP 10    

inline void pthread_function ( void *ptr ){
	pthread_exit(0); 
} 
int main()
{
	int i=0;
	pthread_t thread;
	uint32_t cycles_high, cycles_low, cycles_high1, cycles_low1;
	uint64_t start, end, difference;
	for(i=0;i<OUTER_LOOP;i++){
		asm volatile ("cpuid\n\t"
			      "rdtsc\n\t"
			      "mov %%edx, %0\n\t"
			      "mov %%eax, %1\n\t"
			      : "=r" (cycles_high), "=r" (cycles_low)
				:: "%rax", "%rbx", "%rcx", "%rdx");
		pthread_create (&thread,NULL,(void *) &pthread_function,NULL);
		pthread_join(thread, NULL);
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
=======
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define NUM_LOOP 1000

void pthread_function(void *ptr) {
	pthread_exit(0);
}
>>>>>>> 096b2e3413523ea0bf7189051a25ac261c40e617

int main() {
    double start, end, total_clocks = 0;
    double avg_clock, stddev, variance, sum = 0;
    double clocks[NUM_LOOP] = {0};
    pthread_t thread;
    int i;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

    printf("Loading test module... \n");
    for (i=0; i<NUM_LOOP; i++) {
        printf("Iteration # %d \n", i);
        //preempt_disable();            /*we disable preemption on our CPU*/
        //raw_local_irq_save(flags);    /*we disable hard interrupts on our CPU*/

        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

        // perform the actual operation
        pthread_create(&thread, NULL, (void *)&pthread_function, NULL);
        pthread_join(thread, NULL);

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        //raw_local_irq_restore(flags);     /*we enable hard interrupts on our CPU*/
        //preempt_enable(); /*we enable preemption*/

        start = (double) ( ((uint64_t)cycles_high << 32) | cycles_low );
        end = (double) ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );

        printf("\n function execution time is %f clock cycles \n", (end - start));
        clocks[i] = (end - start);
        total_clocks += clocks[i];
    }

    avg_clock = total_clocks/NUM_LOOP;
    for (i=0; i<NUM_LOOP; i++) {
        sum += pow((clocks[i] - avg_clock), 2);
    }

    variance = sum/(NUM_LOOP - 1);
        stddev = sqrt(variance);
    printf("average clock cycles for time is %f \n", avg_clock);
    printf("variance is %f \n", variance);
    printf("standard deviation is %f \n", stddev);
    return 0;
}
