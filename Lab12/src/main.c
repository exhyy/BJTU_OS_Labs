#include "MemoryAllocation.h"
#include <stdlib.h>
#include <stdio.h>

#define RANDOM_SEED 114514

int main()
{
    printf("20281109-HuangYuyao-OS_Lab12\n");

    srand(RANDOM_SEED);

    MemoryAllocationSimulator simulator;
    int unit_size = 1024;
    int system_size = 128 * 1024 * 1024;
    int user_size = 384 * 1024 * 1024;
    int max_events = 1000;
    int algorithm;
    fprintf(stdout, "请选择动态分区分配算法：\n");
    fprintf(stdout, "  1. 首次适应（First Fit）算法\n");
    fprintf(stdout, "  2. 循环首次适应（Next Fit）算法\n");
    fprintf(stdout, "请输入编号：");
    fscanf(stdin, "%d", &algorithm);
    if (algorithm != 1 && algorithm != 2)
    {
        fprintf(stderr, "请输入1或2！\n");
        return 1;
    }
    init_simulator(&simulator, algorithm, unit_size, system_size, user_size);
    random_simulation(&simulator, max_events);
    fprintf(stdout, "总耗时：%d，总查找次数：%d，平均内存利用率：%.2lf\%%\n", simulator.time, simulator.memory.search_cnt, simulator.memory_usage * 100);
    return 0;
}