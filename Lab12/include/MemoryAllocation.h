#ifndef MEMORYALLOCATION_H_
#define MEMORYALLOCATION_H_

#include "FreeTable.h"

#define FIRST_FIT 1
#define NEXT_FIT 2

typedef struct Memory
{
    int unit_size;
    int system_size;
    int user_size;
    FreeTable free_table;
} Memory;

void init_memory(Memory *memory, int unit_size, int system_size, int user_size);
int allocate_memory(Memory *memory, int size, int algorithm);
void create_process(Memory *memory, int size, int duration);

#endif