#ifndef MEMORYALLOCATION_H_
#define MEMORYALLOCATION_H_

#include "FreeTable.h"

typedef struct Memory
{
    int unit_size;
    int system_size;
    int user_size;
    FreeTable free_table;
} Memory;

void init_memory(Memory *memory, int unit_size, int system_size, int user_size);


#endif