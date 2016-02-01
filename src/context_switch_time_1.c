#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>

struct shareBuff{
    uint32_t cycles_high;
    uint32_t cycles_low;
    uint32_t cycles_high1;
    uint32_t cycles_low1;
};

void* thread2_read(void* sptr_arg){
    struct shareBuff** sptr = (struct shareBuff**) sptr_arg;

    // Take the end time reading
    asm volatile ("rdtscp\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t"
                  "cpuid\n\t"
                  : "=r" ((**sptr).cycles_high1), "=r" ((**sptr).cycles_low1)
                    :: "%rax", "%rbx", "%rcx", "%rdx");
}

void* thread1_write(void* sptr_arg){
    struct shareBuff* sptr = (struct shareBuff*) sptr_arg;
    pthread_t thread_2;

    // Take the start time reading
    asm volatile ("cpuid\n\t"
                  "rdtsc\n\t"
                  "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t"
                  : "=r" (sptr->cycles_high), "=r" (sptr->cycles_low)
                    :: "%rax", "%rbx", "%rcx", "%rdx");

    pthread_create(&thread_2, NULL, thread2_read, &sptr);
    pthread_join(thread_2, NULL);
}

void helper_func(struct shareBuff* sptr){
    sptr->cycles_high  = sptr->cycles_low  = 0;
    sptr->cycles_high1 = sptr->cycles_low1 = 0;
}

int main(){
    uint64_t difference = 0, start = 0, end = 0;
    // Create an instance of shareBuff and populate the values
    struct shareBuff sobj;

    // The pthread variables for creating the two threads
    pthread_t thread_1;

    // Helper function to set the right values for the member variables of shareBuff
    helper_func(&sobj);

    pthread_create(&thread_1, NULL, thread1_write, &sobj);
    pthread_join(thread_1, NULL);

    start = (((uint64_t)sobj.cycles_high << 32) | sobj.cycles_low);
    end   = (((uint64_t)sobj.cycles_high1 << 32) | sobj.cycles_low1);

    difference = end - start;

    printf("The difference is %lu\n", difference);

    return 0;
}
