#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <string.h>

#define RAM_CHECK_ARRAY_SIZE 4 *1024 * 1024 
#define L1_CHECK_ARRAY_SIZE 16 * 1024
#define L2_CHECK_ARRAY_SIZE 256 * 1024
#define L3_CHECK_ARRAY_SIZE 2 * 1024 * 1024



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



void shuffle_indexes(uint32_t* arr,size_t array_size){
    for(size_t i = 0; i < array_size;i++){
        arr[i] = i;
    }
    for(size_t i = 1; i < array_size;i++){
        size_t temp_val = arr[i];
        long rand_index = rand() % (i);
        arr[i] = arr[rand_index];
        arr[rand_index] = temp_val;
    }
}




double get_ram_latency(){
    size_t full_malloc_size = RAM_CHECK_ARRAY_SIZE * sizeof(uint32_t);
    uint32_t* arr = malloc(full_malloc_size);
    if(arr == NULL){
        printf("failed to allocate %d bytes\n",RAM_CHECK_ARRAY_SIZE);
        return 0;
    }
    shuffle_indexes(arr,RAM_CHECK_ARRAY_SIZE);

    uint64_t start,end;

    int array_index = 0;
    start = get_current_tsc();
    for(int i = 0; i < ITERATIONS_NUMBER;i++){
        array_index = arr[array_index];
    }
    end = get_current_tsc();
    free(arr);
    return (double)(end - start) / ITERATIONS_NUMBER;
}







double get_cache_latency(int cache_number){

    size_t size_of_array;
    switch (cache_number){
        case 1:
            size_of_array = L1_CHECK_ARRAY_SIZE;
            break;    
        case 2:
            size_of_array = L2_CHECK_ARRAY_SIZE;
            break;
        case 3:
            size_of_array = L3_CHECK_ARRAY_SIZE;
            break;
        default:
            size_of_array = -1;
            break;
    }
    if(size_of_array == -1)
        return -1.0;


    size_t full_malloc_size = size_of_array * sizeof(uint32_t);
    uint32_t* arr = malloc(full_malloc_size);
    shuffle_indexes(arr,size_of_array);
    volatile int tmp;
    uint64_t start,end;
    start = get_current_tsc();
    for(int i = 0; i < size_of_array;i+=65){
        tmp = arr[tmp];
    }
    end = get_current_tsc();
    free(arr);
    return (double)(end - start) / (size_of_array/65);
}


int main(int argc,char* argv[]){

    if(argc != 2){
        printf("usage : ./program l1/l2/l3/ram\n");
        return 0;
    } 


    pin_to_core(rand() % NUMBER_OF_CORES-1); //cpu affinity

    struct sched_param p = { .sched_priority = 99 };
    sched_setscheduler(0, SCHED_FIFO, &p);
    //prevents from context switching in the middle of the calculation



    double latency_in_cycles;
    if(strcmp(argv[1],"l1") == 0) latency_in_cycles = get_cache_latency(1); 
    else if(strcmp(argv[1],"l2") == 0) latency_in_cycles = get_cache_latency(2);
    else if(strcmp(argv[1],"l3") == 0) latency_in_cycles = get_cache_latency(3);
    else if(strcmp(argv[1],"ram") == 0) latency_in_cycles = get_ram_latency();

    printf("latency (cycles per access): %f\n", latency_in_cycles);
}





