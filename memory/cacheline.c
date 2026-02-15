/**
 * This program demostrates the cache line size of modern CPUs,
 * speedups via cache hits, penalties for cache misses and OS page
 * level misses
*/

#include<stdio.h>
#include<stdint.h>
#include<x86intrin.h>

// 8 * 4 KiB = 8 pages
uint8_t array[4096*8];

// utility to measure CPU cycles
static inline uint64_t rdtscp() {
    unsigned int aux;
    return __rdtscp(&aux);
}

int main()
{
    uint64_t start, end;
    volatile uint8_t *addr = &array; // point to 1st item of array

    _mm_clflush((void*)addr);
    _mm_mfence();

    // First volatile access should take longer cycles due to L1/L2 cache miss
    start = rdtscp();
    *addr;  // should pull 64 bytes of data in L1 cache
    end = rdtscp();

    printf("First access (after flush): %lu cycles\n", end - start);

    // Second access should take fewer cycles than before due to cache hit
    start = rdtscp();
    *addr;
    end = rdtscp();

    printf("Second access: %lu cycles\n", end - start);


    // Access an element within cache line boundary should take same cycles due to cache hit
    start = rdtscp();
    *(addr+63);
    end = rdtscp();

    printf("Accessing element within cache line boundary: %lu cycles\n", end - start);

    // Access an element within cache line boundary should take same cycles due to cache hit
    start = rdtscp();
    *(addr+(4095));
    end = rdtscp();

    printf("Accessing element outside cache line but within page boundary: %lu cycles\n", end - start);

    // Access an element outside of page boundary, takes longer cycle to load whole page
    start = rdtscp();
    *(addr+(4096*2)); // induce page fault
    end = rdtscp();

    printf("Accessing element outside of page boundary: %lu cycles\n", end - start);

    return 0;

}
