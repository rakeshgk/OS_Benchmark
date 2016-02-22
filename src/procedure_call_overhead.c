#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define NUM_TRIALS 1000
#define NUM_LOOPS  1000

void test_procedure_0(){

}

void test_procedure_1(int a){

}

void test_procedure_2(int a, int b){

}

void test_procedure_3(int a, int b, int c){

}

void test_procedure_4(int a, int b, int c, int d){

}

void test_procedure_5(int a, int b, int c, int d, int e){

}

void test_procedure_6(int a, int b, int c, int d, int e, int f){

}

void test_procedure_7(int a, int b, int c, int d, int e, int f, int g){

}

int main() {
    /*
        Uncomment each line of code that involves a function call.
        Call them one after the other to find the overhead of passing arguments
    */
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    uint64_t start, end;
    double difference;
    int i, j;
    FILE* fp;
    fp = fopen("../data/procedure_call_overhead.csv", "w");

    for (i=0; i<NUM_TRIALS; i++) {

        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

        for(j=0; j<NUM_LOOPS; ++j){

            // perform the actual operation
            // The non-inline part of the code call
            test_procedure_0();
            // test_procedure_1(0);
            // test_procedure_2(0, 0);
            // test_procedure_3(0, 0, 0);
            // test_procedure_4(0, 0, 0, 0);
            // test_procedure_5(0, 0, 0, 0, 0);
            // test_procedure_6(0, 0, 0, 0, 0, 0);
            // test_procedure_7(0, 0, 0, 0, 0, 0, 0);
        }

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32) | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
        difference = ((double) (end - start)) / NUM_LOOPS;
        fprintf(fp, "%lf\n", difference);
    }
    fclose(fp);
    return 0;
}
