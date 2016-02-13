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

int main() {
    uint64_t start, end, total_clocks = 0;
    uint64_t avg_clock;

    int i;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

    for (i=0; i<NUM_LOOP; i++) {
        pthread_t thread;
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

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        //raw_local_irq_restore(flags);     /*we enable hard interrupts on our CPU*/
        //preempt_enable(); /*we enable preemption*/
        pthread_join(thread, NULL);

        start = (double) ( ((uint64_t)cycles_high << 32) | cycles_low );
        end = (double) ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );

        printf("Time: %lu\n", (end - start));
        total_clocks += (end - start);
    }

    avg_clock = total_clocks/NUM_LOOP;

    return 0;
}
