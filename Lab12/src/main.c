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
    int max_events = 100;
    init_simulator(&simulator, FIRST_FIT, unit_size, system_size, user_size);
    random_simulation(&simulator, max_events);
    fprintf(stdout, "总耗时：%d，平均内存利用率：%.2lf\%%\n", simulator.time, simulator.memory_usage * 100);
    return 0;
}