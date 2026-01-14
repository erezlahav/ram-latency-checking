#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>


#define ARRAY_SIZE 1 *1024 * 1024 
#define NUMBER_OF_CORES 16
#define ITERATIONS_NUMBER 10000000


static inline uint64_t get_current_tsc(){
    return __rdtsc();
}

void pin_to_core(int core_number){
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core_number, &set);

    pthread_setaffinity_np(pthread_self(), sizeof(set), &set); 
}



void shuffle_indexes(uint32_t* arr){
    for(int i = 0; i < ARRAY_SIZE;i++){
        arr[i] = i;
    }
    for(int i = 1; i < ARRAY_SIZE;i++){
        int temp_val = arr[i];
        long rand_index = rand() % (i);
        arr[i] = arr[rand_index];
        arr[rand_index] = temp_val;
    }
}




int main(){
    pin_to_core(rand() % NUMBER_OF_CORES-1); //cpu affinity

    struct sched_param p = { .sched_priority = 99 };
    sched_setscheduler(0, SCHED_FIFO, &p);
    //prevents from context switching in the middle of the calculation


    uint32_t* arr = malloc(ARRAY_SIZE * sizeof(uint32_t));
    if(arr == NULL){
        printf("failed to allocate %d bytes\n",ARRAY_SIZE);
        return 0;
    }
    shuffle_indexes(arr);

    uint64_t start,end;

    int array_index = 0;
    start = get_current_tsc();
    for(int i = 0; i < ITERATIONS_NUMBER;i++){
        array_index = arr[array_index];
    }
    end = get_current_tsc();
    printf("RAM latency (cycles per access): %f\n", (double)(end - start) / ITERATIONS_NUMBER);
}





