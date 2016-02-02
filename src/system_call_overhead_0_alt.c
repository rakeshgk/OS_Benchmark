#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    uint64_t start = 0, end = 0;
    unsigned cycles_low, cycles_high, cycles_low1, cycles_high1;
    pid_t process_pid;

    asm volatile (
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
        "%rax", "%rbx", "%rcx", "%rdx");

    process_pid = getpid();

    asm volatile(
        "RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
        "%rbx", "%rcx", "%rdx");

    start = ( ((uint64_t)cycles_high << 32) | cycles_low );
    end = ( ((uint64_t)cycles_high1 << 32) | cycles_low1 );
    printf("Time: %lu\n", (end - start));
    return 0;
}
