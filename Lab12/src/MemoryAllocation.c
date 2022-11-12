#include "MemoryAllocation.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

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
    memory->compacted = 0;
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
            if (memory->free_table.data[i].status == PARTITION_FREE && memory->free_table.data[i].size >= size)
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
            PARTITION_FREE};

        // 修改原空闲分区
        memory->free_table.data[free_partition_index].size = allocated_size;
        memory->free_table.data[free_partition_index].status = PARTITION_USED;

        // 将新的空闲分区插入到新占用分区的后面
        insert_free_table_item(&(memory->free_table), new_partition, free_partition_index);
        memory->compacted = 0;
        return free_partition_index;
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
            compact_memory(memory);
            int partition_index = allocate_memory(memory, size, algorithm);
            memory->compacted = 1;
            return partition_index;
        }
        else
        {
            // TODO: 挂起进程，返回-1表示分配失败
            memory->compacted = 0;
            return -1;
        }
    }
}

void compact_memory(Memory *memory)
{
    int max_partition_index = 0;
    int size_total = memory->system_size;

    // 0号分区是系统内存，必定被使用
    for (int i = 1; i < memory->free_table.length; i++)
    {
        if (memory->free_table.data[i].status == PARTITION_USED)
        {
            int index = max_partition_index + 1;
            memory->free_table.data[index].status = PARTITION_USED;
            memory->free_table.data[index].size = memory->free_table.data[i].size;
            memory->free_table.data[index].address = memory->free_table.data[max_partition_index].address + memory->free_table.data[max_partition_index].size;
            size_total += memory->free_table.data[index].size;
            max_partition_index = index;
        }
    }

    // 合并剩下的分区
    for (int i = memory->free_table.length - 1; i > max_partition_index; i--)
    {
        remove_free_table_item(&(memory->free_table), i);
    }
    int new_free_size = memory->system_size + memory->user_size - size_total;
    int new_free_address = memory->free_table.data[max_partition_index].address + memory->free_table.data[max_partition_index].size;
    FreeTableItem new_free_partition = {new_free_size, new_free_address, PARTITION_FREE};
    insert_free_table_item(&(memory->free_table), new_free_partition, max_partition_index);
}

void recycle_memory(Memory *memory, int partition_id)
{
    if (memory->free_table.data[partition_id].status == PARTITION_FREE)
        return;
    if (partition_id + 1 < memory->free_table.length)
    {
        if (memory->free_table.data[partition_id - 1].status == PARTITION_FREE && memory->free_table.data[partition_id + 1].status == PARTITION_USED)
        {
            // 与前一格空闲分区相邻
            int new_size = memory->free_table.data[partition_id - 1].size + memory->free_table.data[partition_id].size;
            memory->free_table.data[partition_id - 1].size = new_size;
            remove_free_table_item(&(memory->free_table), partition_id);
        }
        else if (memory->free_table.data[partition_id - 1].status == PARTITION_USED && memory->free_table.data[partition_id + 1].status == PARTITION_FREE)
        {
            // 与后一格空闲分区相邻
            int new_size = memory->free_table.data[partition_id + 1].size + memory->free_table.data[partition_id].size;
            memory->free_table.data[partition_id].size = new_size;
            memory->free_table.data[partition_id].status = PARTITION_FREE;
            remove_free_table_item(&(memory->free_table), partition_id + 1);
        }
        else if (memory->free_table.data[partition_id - 1].status == PARTITION_FREE && memory->free_table.data[partition_id + 1].status == PARTITION_FREE)
        {
            // 与前后一格的空闲分区相邻
            int new_size = memory->free_table.data[partition_id - 1].size + memory->free_table.data[partition_id].size + memory->free_table.data[partition_id + 1].size;
            memory->free_table.data[partition_id - 1].size = new_size;
            remove_free_table_item(&(memory->free_table), partition_id + 1);
            remove_free_table_item(&(memory->free_table), partition_id);
        }
        else
        {
            // 不与空闲分区相邻
            memory->free_table.data[partition_id].status = PARTITION_FREE;
        }
    }
    else
    {
        if (memory->free_table.data[partition_id - 1].status == PARTITION_FREE)
        {
            // 与前一格空闲分区相邻
            int new_size = memory->free_table.data[partition_id - 1].size + memory->free_table.data[partition_id].size;
            memory->free_table.data[partition_id - 1].size = new_size;
            remove_free_table_item(&(memory->free_table), partition_id);
        }
        else
        {
            // 不与空闲分区相邻
            memory->free_table.data[partition_id].status = PARTITION_FREE;
        }
    }
}

void init_simulator(MemoryAllocationSimulator *simulator, int algorithm, int unit_size, int system_size, int user_size)
{
    memset(simulator->process, 0, sizeof(simulator->process));
    simulator->num_process = 0;
    simulator->time = 0;
    simulator->algorithm = algorithm;
    init_memory(&(simulator->memory), unit_size, system_size, user_size);
    init_queue(&(simulator->suspended_queue));
}

int cmp_pair(const void *p1, const void *p2)
{
    const Pair *a = (const Pair *)p1;
    const Pair *b = (const Pair *)p2;
    return a->first > b->first;
}

void create_process(MemoryAllocationSimulator *simulator, int size, int duration)
{
    fprintf(stdout, "正在尝试创建进程——内存%.2lfKB，持续时间%d...", (double)size / 1024, duration);
    int index = -1;
    // 查找第1个已经结束的进程
    for (int i = 0; i < NUM_PROCESS_MAX; i++)
    {
        if (simulator->process[i].status == PROCESS_FINISHED)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
    {
        fprintf(stdout, "\n");
        fprintf(stderr, "错误：进程数已达上限（%d）\n", NUM_PROCESS_MAX);
        return;
    }

    int memory_index = allocate_memory(&(simulator->memory), size, simulator->algorithm);
    if (memory_index == -1)
    {
        // 挂起进程
        fprintf(stdout, "创建失败，进程挂起\n");
        simulator->process[index].status = PROCESS_SUSPENDED;
        simulator->process[index].memory_size = size;
        simulator->process[index].time = duration;
        queue_push(&(simulator->suspended_queue), index);
    }
    else
    {
        simulator->process[index].memory_index = memory_index;
        simulator->process[index].memory_size = size;
        simulator->process[index].time = duration;
        simulator->process[index].status = PROCESS_RUNNING;
        if (simulator->memory.compacted == 1)
        {
            // 发生了紧凑，更新所有进程的memory_index
            fprintf(stdout, "完成紧凑...");
            int cnt = 0;
            Pair id_pairs[NUM_PROCESS_MAX]; // {memory_id, process_id}
            for (int i = 0; i < NUM_PROCESS_MAX; i++)
            {
                if (simulator->process[i].status == PROCESS_RUNNING && i != index)
                {
                    id_pairs[cnt].first = simulator->process[i].memory_index;
                    id_pairs[cnt].second = i;
                    cnt++;
                }
            }
            qsort(id_pairs, cnt, sizeof(Pair), cmp_pair);
            for (int i = 0; i < cnt; i++)
            {
                // 0号分区是系统内存，因此从1开始
                simulator->process[id_pairs[i].second].memory_index = i + 1;
            }
        }
        fprintf(stdout, "成功分配内存%.2lfKB\n", simulator->memory.free_table.data[memory_index].size / 1024.0);
    }
}

void finish_process(MemoryAllocationSimulator *simulator, int process_id)
{
    int memory_index = simulator->process[process_id].memory_index;
    fprintf(stdout, "回收进程%d的内存%.2lfKB\n", process_id, simulator->memory.free_table.data[memory_index].size / 1024.0);
    simulator->process[process_id].status = PROCESS_FINISHED;
    recycle_memory(&(simulator->memory), memory_index);
}

int activate_process(MemoryAllocationSimulator *simulator)
{
    if (queue_empty(&(simulator->suspended_queue)))
        return -1;

    int process_id = queue_pop(&(simulator->suspended_queue));
    int memory_index = allocate_memory(&(simulator->memory), simulator->process[process_id].memory_size, simulator->algorithm);
    if (memory_index == -1)
    {
        return -1;
    }

    simulator->process[process_id].memory_index = memory_index;
    simulator->process[process_id].status = PROCESS_RUNNING;
    if (simulator->memory.compacted == 1)
    {
        // 发生了紧凑，更新所有进程的memory_index
        int cnt = 0;
        Pair id_pairs[NUM_PROCESS_MAX]; // {memory_id, process_id}
        for (int i = 0; i < NUM_PROCESS_MAX; i++)
        {
            if (simulator->process[i].status == PROCESS_RUNNING && i != process_id)
            {
                id_pairs[cnt].first = simulator->process[i].memory_index;
                id_pairs[cnt].second = i;
                cnt++;
            }
        }
        qsort(id_pairs, cnt, sizeof(Pair), cmp_pair);
        for (int i = 0; i < cnt; i++)
        {
            // 0号分区是系统内存，因此从1开始
            simulator->process[id_pairs[i].second].memory_index = i + 1;
        }
    }
    return process_id;
}

void random_simulation(MemoryAllocationSimulator *simulator, int max_events)
{
    simulator->time = 0;
    int cnt_event = 0;
    while (1)
    {
        simulator->time++;
        // 所有进程剩余时间减1，将剩余时间为0的进程结束
        for (int i = 0; i < NUM_PROCESS_MAX; i++)
        {
            if (simulator->process[i].status == PROCESS_RUNNING)
            {
                simulator->process[i].time--;
                if (simulator->process[i].time == 0)
                {
                    finish_process(simulator, i);
                    print_free_table(&(simulator->memory.free_table));
                    cnt_event++;
                    if (cnt_event > max_events)
                        break;
                }
            }
        }
        if (cnt_event > max_events)
            break;
        
        int random_num = rand();
        if (random_num % 2 == 0)
            continue;

        int activated_process_id = activate_process(simulator);
        if (activated_process_id != -1) // 尝试激活进程
        {
            cnt_event++;
            Process process = simulator->process[activated_process_id];
            fprintf(stdout, "激活进程%d，内存分区号%d，持续时间%d\n", activated_process_id, process.memory_index, process.time);
            print_free_table(&(simulator->memory.free_table));
            if (cnt_event > max_events)
                break;
        }
        int random_size = rand() % (1024 * 1024 * 64) + 1; // 1B~64MB
        int random_duration = rand() % 100 + 1; // 1~100
        create_process(simulator, random_size, random_duration);
        print_free_table(&(simulator->memory.free_table));
        cnt_event++;
        if (cnt_event > max_events)
            break;
    }
}