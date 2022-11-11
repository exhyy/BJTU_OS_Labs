#include "MemoryAllocation.h"
#include <stdio.h>

void init_memory(Memory *memory, int unit_size, int system_size, int user_size)
{
    if (system_size % unit_size != 0)
    {
        fprintf(stderr, "错误：系统空间（system_size）不是单位空间（unit_size）的整数倍\n");
        return;
    }
    if (user_size % unit_size != 0)
    {
        fprintf(stderr, "错误：用户空间（user_size）不是单位空间（unit_size）的整数倍\n");
        return;
    }
    memory->unit_size = unit_size;
    memory->system_size = system_size;
    memory->user_size = user_size;
    init_free_table(&(memory->free_table), 2);
    FreeTableItem system = {system_size, 0, PARTITION_USED};
    FreeTableItem user = {user_size, system_size, PARTITION_FREE};
    set_free_table_item(&(memory->free_table), system, 0, 1);
    set_free_table_item(&(memory->free_table), user, 1, 1);
}