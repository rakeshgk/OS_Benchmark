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
    double start, end, start_temp, total_clocks = 0;
    double avg_clock, stddev, variance, sum = 0;
    double clocks[NUM_LOOP] = {0};
    char string[1] = "";
    char readbuffer[1];
    int i, nbytes;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;

    // fd[0] is set up for reading and fd[1] for writing
    int fd[2],childReturnStatus;
    pid_t process_pid;

    for (i=0; i<NUM_LOOP; i++) {
        printf("Iteration # %d \n", i);
        //preempt_disable();            /*we disable preemption on our CPU*/
        //raw_local_irq_save(flags);    /*we disable hard interrupts on our CPU*1
        // Setup the pipe
        pipe(fd);
        // Send a "string" through the output side of the pipe
        write(fd[1], string, (strlen(string) + 1));
        process_pid = fork();

        if(process_pid == 0){
            exit(0);
        }

        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

	    // waitpid(process_pid, &childReturnStatus, 0);
	    start_temp = (double) ( ((uint64_t)cycles_high << 32) | cycles_low );
        printf("Run: %d, start: %f\n", i, start);
        nbytes = read(fd[0], readbuffer, sizeof(readbuffer));

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
