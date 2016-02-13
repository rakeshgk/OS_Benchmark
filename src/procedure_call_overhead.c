#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define NUM_LOOP 1000

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
    int i;
    FILE* fp;
    fp = fopen("../data/procedure_call_overhead.csv", "w");

    for (i=0; i<NUM_LOOP; i++) {
        asm volatile (
            "CPUID\n\t"
            "RDTSC\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
            "%rax", "%rbx", "%rcx", "%rdx");

        // perform the actual operation
        // The non-inline part of the code call
        test_procedure_0();
        // test_procedure_1(1);
        // test_procedure_2(1, 2);
        // test_procedure_3(1, 2, 3);
        // test_procedure_4(1, 2, 3, 4);
        // test_procedure_5(1, 2, 3, 4, 5);
        // test_procedure_6(1, 2, 3, 4, 5, 6);
        // test_procedure_7(1, 2, 3, 4, 5, 6, 7);

        asm volatile(
            "RDTSCP\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
            "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32) | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);
        fprintf(fp, "%ld,%ld\n", start, end);
    }
    fclose(fp);
    return 0;
}
