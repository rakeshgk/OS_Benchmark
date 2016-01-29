#include <stdio.h>
#include <stdint.h>

#define OUTER_LOOP 10

void test_procedure_0(){
    int sum = 0;
}

void test_procedure_1(int a){
    int sum = a;
}

void test_procedure_2(int a, int b){
    int sum = a + b;
}

void test_procedure_3(int a, int b, int c){
    int sum = a + b + c;
}

void test_procedure_4(int a, int b, int c, int d){
    int sum = a + b + c + d;
}

void test_procedure_5(int a, int b, int c, int d, int e){
    int sum = a + b + c + d + e;
}

void test_procedure_6(int a, int b, int c, int d, int e, int f){
    int sum = a + b + c + d + e + f;
}

void test_procedure_7(int a, int b, int c, int d, int e, int f, int g){
    int sum = a + b + c + d + e + f + g
}

extern void inline_test_procedure_0(){
    int sum = 0;
}

extern void inline_test_procedure_1(int a){
    int sum = a;
}

extern void inline_test_procedure_2(int a, int b){
    int sum = a + b;
}

extern void inline_test_procedure_3(int a, int b, int c){
    int sum = a + b + c;
}

extern void inline_test_procedure_4(int a, int b, int c, int d){
    int sum = a + b + c + d;
}

extern void inline_test_procedure_5(int a, int b, int c, int d, int e){
    int sum = a + b + c + d + e;
}

extern void inline_test_procedure_6(int a, int b, int c, int d, int e, int f){
    int sum = a + b + c + d + e + f;
}

extern void inline_test_procedure_7(int a, int b, int c, int d, int e, int f, int g){
    int sum = a + b + c + d + e + f + g
}

int main(){
    int i = 0;
    uint32_t cycles_high, cycles_low, cycles_high1, cycles_low1;
    uint64_t start, end, difference;

    for(i=0; i<OUTER_LOOP; ++i){
        asm volatile ("cpuid\n\t"
                      "rdtsc\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t"
                      : "=r" (cycles_high), "=r" (cycles_low)
                        :: "%rax", "%rbx", "%rcx", "%rdx");

        // The non-inline part of the code call
        test_procedure_0();
        // test_procedure_1(1);
        // test_procedure_2(1, 2);
        // test_procedure_3(1, 2, 3);
        // test_procedure_4(1, 2, 3, 4);
        // test_procedure_5(1, 2, 3, 4, 5);
        // test_procedure_6(1, 2, 3, 4, 5, 6);
        // test_procedure_7(1, 2, 3, 4, 5, 6, 7);

        // The inline part of the code call
        // inline_test_procedure_0
        // inline_test_procedure_1(1);
        // inline_test_procedure_2(1, 2);
        // inline_test_procedure_3(1, 2, 3);
        // inline_test_procedure_4(1, 2, 3, 4);
        // inline_test_procedure_5(1, 2, 3, 4, 5);
        // inline_test_procedure_6(1, 2, 3, 4, 5, 6);
        // inline_test_procedure_7(1, 2, 3, 4, 5, 6, 7);

        asm volatile ("rdtscp\n\t"
                      "mov %%edx, %0\n\t"
                      "mov %%eax, %1\n\t"
                      "cpuid\n\t"
                      : "=r" (cycles_high1), "=r" (cycles_low1)
                        :: "%rax", "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)cycles_high << 32)  | cycles_low);
        end   = (((uint64_t)cycles_high1 << 32) | cycles_low1);

        difference = (end - start);

        printf("The difference is - %lu\n", difference);
    }

    return 0;
}
