#include "MemoryAllocation.h"
#include <stdio.h>
#include <math.h>

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

int allocate_memory(Memory *memory, int size, int algorithm)
{
    int free_partition_index = -1;
    if (algorithm == FIRST_FIT)
    {
        for (int i = 0; i < memory->free_table.length; i++)
        {
            if (memory->free_table.data[i].status == PARTITION_FREE)
            {
                free_partition_index = i;
                break;
            }
        }
    }
    else if (algorithm == NEXT_FIT)
    {
        static int current = -1;
        int i = current + 1;
        while (i != current)
        {
            if (memory->free_table.data[i].status == PARTITION_FREE)
            {
                free_partition_index = i;
                current = i;
                break;
            }
            i = (i + 1) % memory->free_table.length;
        }
    }
    else
    {
        fprintf(stderr, "错误：未知动态分区分配算法\n");
    }

    if (free_partition_index != -1)
    {
        int allocated_size = (int)ceil((double)size / memory->unit_size) * memory->unit_size;
        int new_size = memory->free_table.data[free_partition_index].size - allocated_size;
        int new_address = memory->free_table.data[free_partition_index].address + allocated_size;
        FreeTableItem new_partition = {
            new_size,
            new_address,
            PARTITION_USED};

        // 修改原空闲分区
        memory->free_table.data[free_partition_index].size = allocated_size;
        memory->free_table.data[free_partition_index].status = PARTITION_USED;

        // 将新的空闲分区插入到新占用分区的后面
        insert_free_table_item(&(memory->free_table), new_partition, free_partition_index);
    }
    else
    {
        int free_total = 0; // 总空闲空间
        for (int i = 0; i < memory->free_table.length; i++)
        {
            if (memory->free_table.data[i].status == PARTITION_FREE)
            {
                free_total += memory->free_table.data[i].size;
            }
        }
        if (free_total >= size)
        {
            // TODO: 进行紧凑，然后重新尝试分配内存（可以尝试递归？）
        }
        else
        {
            // TODO: 挂起进程，返回-1表示分配失败
        }
    }
}