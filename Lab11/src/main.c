#include "deadlock.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// 仅在随机场景生成中使用
#define NUM_PROCESS_RNAD_MAX 5
#define NUM_RESOURCE_RAND_MAX 5
#define RESOURCE_RAND_MAX 3

int main()
{
    printf("20281109-HuangYuyao-OS_Lab11\n");

    int num_process, num_res;
    Process *process;
    int *allocation, *request, *available;
    printf("请输入进程个数（0表示随机生成场景）：");
    scanf("%d", &num_process);
    if (num_process > 0)
    {
        // 手动输入
        process = (Process *)malloc(num_process * sizeof(Process));
        printf("请输入资源类别数：");
        scanf("%d", &num_res);
        for (int i = 0; i < num_process; i++)
            init_process(&process[i], num_res);
        allocation = (int *)malloc(num_res * sizeof(int));
        request = (int *)malloc(num_res * sizeof(int));
        available = (int *)malloc(num_res * sizeof(int));
        for (int i = 0; i < num_process; i++)
        {
            printf("请输入进程%d的资源分配数（%d个整数）：", i, num_res);
            for (int j = 0; j < num_res; j++)
                scanf("%d", &allocation[j]);
            set_process_allocation(&process[i], allocation);

            printf("请输入进程%d的资源请求数（%d个整数）：", i, num_res);
            for (int j = 0; j < num_res; j++)
                scanf("%d", &request[j]);
            set_process_request(&process[i], request);
        }
        printf("请输入可用资源数（%d个整数）：", num_res);
        for (int j = 0; j < num_res; j++)
            scanf("%d", &available[j]);
    }
    else if (num_process == 0)
    {
        // 随机生成
        srand((int)(time(0)));
        num_process = rand() % NUM_PROCESS_RNAD_MAX + 1;
        num_res = rand() % NUM_RESOURCE_RAND_MAX + 1;

        process = (Process *)malloc(num_process * sizeof(Process));
        allocation = (int *)malloc(num_res * sizeof(int));
        request = (int *)malloc(num_res * sizeof(int));
        available = (int *)malloc(num_res * sizeof(int));

        for (int i = 0; i < num_process; i++)
        {
            init_process(&process[i], num_res);
            for (int j = 0; j < num_res; j++)
            {
                allocation[j] = rand() % RESOURCE_RAND_MAX;
                request[j] = rand() % RESOURCE_RAND_MAX;
            }
            set_process_allocation(&process[i], allocation);
            set_process_request(&process[i], request);
        }
        for (int j = 0; j < num_res; j++)
            available[j] = rand() % RESOURCE_RAND_MAX + 1;

        printf("随机生成的场景如下：\n");
        printf("进程数：%d，资源类别数：%d\n", num_process, num_res);
        for (int i = 0; i < num_process; i++)
        {
            printf("进程%d\n", i);
            printf("  资源分配数：");
            for (int j = 0; j < num_res; j++)
                printf("%d ", process[i].allocation[j]);
            printf("\n");

            printf("  资源请求数：");
            for (int j = 0; j < num_res; j++)
                printf("%d ", process[i].request[j]);
            printf("\n");
        }
        printf("可用资源数：");
        for (int j = 0; j < num_res; j++)
            printf("%d ", available[j]);
        printf("\n");
    }
    else
    {
        printf("错误：请输入非负数！\n");
        exit(1);
    }

    DeadlockTask deadlock_task;
    init_deadlock_task(&deadlock_task, num_res, num_process);
    set_task_process(&deadlock_task, process);
    set_task_available(&deadlock_task, available);
    puts("check1");
    if (check_deadlock(&deadlock_task))
        puts("发生死锁！");
    else
        puts("未发生死锁！");

    return 0;
}