#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define NUM_LOOP 10000

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

extern void inline_test_procedure_0(){

}

extern void inline_test_procedure_1(int a){

}

extern void inline_test_procedure_2(int a, int b){

}

extern void inline_test_procedure_3(int a, int b, int c){

}

extern void inline_test_procedure_4(int a, int b, int c, int d){

}

extern void inline_test_procedure_5(int a, int b, int c, int d, int e){

}

extern void inline_test_procedure_6(int a, int b, int c, int d, int e, int f){

}

extern void inline_test_procedure_7(int a, int b, int c, int d, int e, int f, int g){

}

int main() {
    double start, end, total_clocks = 0;
    double avg_clock, stddev, variance, sum = 0;
    double clocks[NUM_LOOP] = {0};
    //int variable = 0;
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
