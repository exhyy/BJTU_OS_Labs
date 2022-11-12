#ifndef MEMORYALLOCATION_H_
#define MEMORYALLOCATION_H_

#include "FreeTable.h"
#include "utils.h"

#define FIRST_FIT 1
#define NEXT_FIT 2

typedef struct Memory
{
    int unit_size;
    int system_size;
    int user_size;
    int compacted; // 用来标记是否发生了紧凑
    FreeTable free_table;
} Memory;

void init_memory(Memory *memory, int unit_size, int system_size, int user_size);
int allocate_memory(Memory *memory, int size, int algorithm);
void recycle_memory(Memory *memory, int partition_id);
void compact_memory(Memory *memory);

#define PROCESS_FINISHED 0
#define PROCESS_RUNNING 1
#define PROCESS_SUSPENDED 2

typedef struct Process
{
    int memory_index;
    int memory_size;
    int status;
    int time;
} Process;

#define NUM_PROCESS_MAX 64

typedef struct MemoryAllocationSimulator
{
    Process process[NUM_PROCESS_MAX];
    int time;
    int algorithm;
    double memory_usage;
    Memory memory;
    Queue suspended_queue;
} MemoryAllocationSimulator;

void init_simulator(MemoryAllocationSimulator *simulator, int algorithm, int unit_size, int system_size, int user_size);
void create_process(MemoryAllocationSimulator *simulator, int size, int duration);
void finish_process(MemoryAllocationSimulator *simulator, int process_id);
int activate_process(MemoryAllocationSimulator *simulator);
void random_simulation(MemoryAllocationSimulator *simulator, int max_events);

#endif