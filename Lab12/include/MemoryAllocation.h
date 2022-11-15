#ifndef MEMORYALLOCATION_H_
#define MEMORYALLOCATION_H_

#include "FreeTable.h"
#include "utils.h"

#define FIRST_FIT 1
#define NEXT_FIT 2

typedef struct Memory
{
    int unit_size;   // 单位空间大小
    int system_size; // 系统空间大小
    int user_size;   // 用户空间大小
    int compacted;   // 用来标记是否发生了紧凑
    int search_cnt;  // 查找计数
    FreeTable free_table;
} Memory;

void init_memory(Memory *memory, int unit_size, int system_size, int user_size);
int allocate_memory(Memory *memory, int size, int algorithm);
int recycle_memory(Memory *memory, int partition_id);
void compact_memory(Memory *memory);

#define PROCESS_FINISHED 0
#define PROCESS_RUNNING 1
#define PROCESS_SUSPENDED 2

typedef struct Process
{
    int memory_index; // 内存分区号
    int memory_size;  // 所占内存大小
    int status;       // 进程状态
    int time;         // 剩余时间
} Process;

#define NUM_PROCESS_MAX 64

typedef struct MemoryAllocationSimulator
{
    Process process[NUM_PROCESS_MAX];
    int time;            // 当前耗时
    int algorithm;       // 分配算法
    double memory_usage; // 内存利用率
    Memory memory;
    Queue suspended_queue; // 挂起队列
} MemoryAllocationSimulator;

void init_simulator(MemoryAllocationSimulator *simulator, int algorithm, int unit_size, int system_size, int user_size);
void create_process(MemoryAllocationSimulator *simulator, int size, int duration);
void finish_process(MemoryAllocationSimulator *simulator, int process_id);
int activate_process(MemoryAllocationSimulator *simulator);
void random_simulation(MemoryAllocationSimulator *simulator, int max_events);

#endif