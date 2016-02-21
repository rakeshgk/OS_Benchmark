#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define KB 1024
#define MB 1024*KB
#define MIN_LEN 512
#define MAX_LEN 256*MB
#define NUM_SAMPLE 100
#define TEST_CNT 100000
#define WALK_CNT 100000
#define NUM_STRIDE 6
#define STRIDE {8, 16, 32, 64, 128, 256}

#define WALK1 p = (long *)*p;
#define WALK10 WALK1 WALK1 WALK1 WALK1 WALK1 WALK1 WALK1 WALK1 WALK1 WALK1
#define WALK100 WALK10 WALK10 WALK10 WALK10 WALK10 WALK10 WALK10 WALK10 WALK10 WALK10
#define WALK1K WALK100 WALK100 WALK100 WALK100 WALK100 WALK100 WALK100 WALK100 WALK100 WALK100
#define WALK10K WALK1K WALK1K WALK1K WALK1K WALK1K WALK1K WALK1K WALK1K WALK1K WALK1K
#define WALK100K WALK10K WALK10K WALK10K WALK10K WALK10K WALK10K WALK10K WALK10K WALK10K WALK10K

/*
void setup_test(long *arr, int size, int stride) {
    int i;
    printf("Setting up for size: %d, stride: %d \n", size, stride);
    for(i = stride; i < size; i += stride) {
        arr[i] = (long) &arr[(i-stride)%size];
    }
    arr[0] = (long) &arr[i - stride];
    //p = (char**)&addr[0];
}

void warmup_test(long *arr, int size, int stride) {
    int i;
    long *p = arr;
    printf("Warming up for size: %d, stride: %d \n", size, stride);
    for (i=0; i<WARMUP_CNT; i++) {
            p = (long *)*p;
    }
}

uint64_t loop_overhead() {
    uint32_t cycles_low, cycles_high, cycles_low1, cycles_high1;
    uint64_t start, end;
    int i;

    asm volatile (
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
        "%rax", "%rbx", "%rcx", "%rdx");

    for (i=0; i<TEST_CNT; i++) {
        // NO OP
    }

    asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");
    start = (((uint64_t)cycles_high << 32) | cycles_low);
    end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
    return (end - start);
}
*/

void latency_test(long *arr, long size, int stride, FILE *fp) {
    int i;
    //register int j;
    //register size_t count = size/stride + 1;
    uint32_t cycles_low, cycles_high, cycles_low1, cycles_high1;
    uint64_t start, end, lat_time;
    long *p = NULL;

    printf("Testing for arr of size: %lu, stride: %d\n", size, stride);

    //loop_oh = loop_overhead();

    printf("Setting up for size: %lu, stride: %d \n", size, stride);
    for(i = stride; i < size; i += stride) {
        arr[i] = (long) &arr[(i-stride)%size];
    }
    arr[0] = (long) &arr[i - stride];

    p = &arr[0];

    printf("Warming up for size: %lu, stride: %d \n", size, stride);
    WALK1K;

    for(i = 0; i < NUM_SAMPLE; i++) {
        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

/*
        for(j=0; j < TEST_CNT; j++) {
            // Do work
            // printf("read from  %lu \n", (long)p);
            p = (long *)*p;
        }
*/

        WALK100K;

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32) | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
        lat_time = end - start;

        fprintf(fp, "%d, %lu, %d, %lu\n", (int)i, size*8, stride*8, lat_time/WALK_CNT);

    }
}

int main() {
    int i;
    long size;
    int stride[NUM_STRIDE] = STRIDE;
    long *arr = NULL;

    FILE* fp;
    fp = fopen("../data/memory_latency_0.csv", "w");

    // Allocate 256MB of memory chunk
    arr = (long *)malloc(MAX_LEN * sizeof(long));

    if(!arr) {
        printf("Failed to allocate array of size: %d \n", MAX_LEN);
        return 0;
    }

    for(i = 0; i < NUM_STRIDE; i++) {
        for(size = MIN_LEN; size <= MAX_LEN; size *= 2) {
            if (size > stride[i]) {
                //setup_test(arr, size, stride[i]);
                //warmup_test(arr, size, stride[i]);
                latency_test(arr, size, stride[i], fp);
            }
        }
    }
    free(arr);
}
