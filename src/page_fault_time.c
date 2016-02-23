#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

/*
void create_random_data_file(const int page_size){
    int buffer_size = 0;
    char* buf = NULL;
    int index = 0;

    FILE* fp = fopen("../data/random_data_file.txt", "w");

    buffer_size = sizeof(char) * page_size;
    buf = (char *)malloc(buffer_size);

    for(index = 0; index < buffer_size; ++index){
        if(index % 2 == 0){
            buf[index] = 'a';
        }
        else{
            buf[index] = 'b';
        }
    }

    fwrite(buf, 1, buffer_size, fp);
    fclose(fp);
}
*/

int main(){
    /*
        1. Get the page size of the system on which the program is running
        2. Create a random file that contains so many bytes of data
        3. Load the random data in to memory through mmap
        4. Time this operation
        5. Free the allocated space through munmap
    */

    int index = 0;
    struct stat statbuf;
    int fd = 0;
    void* ret_addr;
    char buf[4097];
    uint32_t cycles_low, cycles_high;
    uint32_t cycles_low1, cycles_high1;
    uint64_t start, end;
    FILE* fp = NULL;

    int page_size = 0;
    page_size = getpagesize();

    // Store the results in this file
    fp = fopen("../data/page_fault_time.csv", "a");

    // Creating the random file
    // create_random_data_file(page_size);

    // Obtaining the file stats using the stat buffer
    stat("../data/random_data_file.txt", &statbuf);

    // Open the random data file in read mode for local access
    fd = open("../data/random_data_file.txt", O_RDONLY);

    asm volatile (
        "CPUID\n\t"
        "RDTSC\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
        "%rax", "%rbx", "%rcx", "%rdx");

    // First allocate the page through mmap and read the data into buf
    ret_addr = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    sscanf((char *)ret_addr, "%s", buf);

    asm volatile(
        "RDTSCP\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "CPUID\n\t": "=r" (cycles_high1), "=r" (cycles_low1):: "%rax",
        "%rbx", "%rcx", "%rdx");

    if(ret_addr == MAP_FAILED){
        printf("Error encountered while mapping memory pages through mmap\n");
        exit(1);
    }

    start = (((uint64_t) cycles_high << 32) | cycles_low);
    end   = (((uint64_t) cycles_high1 << 32) | cycles_low1);
    fprintf(fp, "%lu,%lu\n", start, end);

    // Deallocate the memory allocated so that we can allocate it again
    if(munmap(ret_addr, statbuf.st_size) == -1){
        printf("Unable to deallocate the memory pages\n");
        exit(1);
    }

    fclose(fp);
    close(fd);
    return 0;
}
