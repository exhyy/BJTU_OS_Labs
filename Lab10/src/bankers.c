#include "bankers.h"
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void init_process(Process *process, int num_res)
{
    process->num_res = num_res;
    process->max = (int *)malloc(num_res * sizeof(int));
    process->allocation = (int *)malloc(num_res * sizeof(int));
    process->need = (int *)malloc(num_res * sizeof(int));
}

void destroy_process(Process *process)
{
    process->num_res = -1;
    free(process->max);
    free(process->allocation);
    free(process->need);
}

void set_process_max(Process *process, ...)
{
    va_list p_args;
    va_start(p_args, process);
    int arg;
    for (int i = 0; i < process->num_res; i++)
    {
        arg = va_arg(p_args, int);
        process->max[i] = arg;
    }
    va_end(p_args);
}

void set_process_allocation(Process *process, ...)
{
    va_list p_args;
    va_start(p_args, process);
    int arg;
    for (int i = 0; i < process->num_res; i++)
    {
        arg = va_arg(p_args, int);
        process->allocation[i] = arg;
    }
    va_end(p_args);
}

void set_process_need(Process *process, ...)
{
    va_list p_args;
    va_start(p_args, process);
    int arg;
    for (int i = 0; i < process->num_res; i++)
    {
        arg = va_arg(p_args, int);
        process->need[i] = arg;
    }
    va_end(p_args);
}

void init_bankers_task(BankersTask *bankers_task, int num_res, int num_process)
{
    bankers_task->num_res = num_res;
    bankers_task->num_process = num_process;
    bankers_task->process = (Process *)malloc(num_process * sizeof(Process));
    bankers_task->available = (int *)malloc(num_res * sizeof(int));
    bankers_task->work = (int *)malloc(num_res * sizeof(int));
    bankers_task->finish = (int *)malloc(num_process * sizeof(int));
    bankers_task->safe_list = (int *)malloc(num_process * sizeof(int));
}

void destroy_bankers_task(BankersTask *bankers_task)
{
    bankers_task->num_process = -1;
    bankers_task->num_res = -1;
    free(bankers_task->process);
    free(bankers_task->available);
    free(bankers_task->work);
    free(bankers_task->finish);
    free(bankers_task->safe_list);
}

void set_task_process(BankersTask *bankers_task, ...)
{
    va_list p_args;
    va_start(p_args, bankers_task);
    Process arg;
    for (int i = 0; i < bankers_task->num_process; i++)
    {
        arg = va_arg(p_args, Process);
        bankers_task->process[i] = arg;
    }
    va_end(p_args);
}

void set_task_available(BankersTask *bankers_task, ...)
{
    va_list p_args;
    va_start(p_args, bankers_task);
    int arg;
    for (int i = 0; i < bankers_task->num_res; i++)
    {
        arg = va_arg(p_args, int);
        bankers_task->available[i] = arg;
    }
    va_end(p_args);
}

void resource_request(BankersTask *bankers_task, int process_id, ...)
{
    va_list p_args;
    int arg;

    // 打印请求信息
    fprintf(stdout, "尝试处理资源请求，进程%d请求资源数：", process_id);
    va_start(p_args, process_id);
    for (int i = 0; i < bankers_task->num_res; i++)
    {
        arg = va_arg(p_args, int);
        fprintf(stdout, "%d ", arg);
    }
    fprintf(stdout, "\n");
    va_end(p_args);

    // 检查Request[i] <= Need[i]
    va_start(p_args, process_id);
    for (int i = 0; i < bankers_task->num_res; i++)
    {
        arg = va_arg(p_args, int);
        if (arg > bankers_task->process[process_id].need[i])
        {
            fprintf(stderr, "请求拒绝：Request[%d] > Need[%d]\n", i, i);
            return;
        }
    }
    va_end(p_args);

    // 检查Request[i] <= Available[i]
    va_start(p_args, process_id);
    for (int i = 0; i < bankers_task->num_res; i++)
    {
        arg = va_arg(p_args, int);
        if (arg > bankers_task->available[i])
        {
            fprintf(stderr, "请求拒绝：Request[%d] > Available[%d]\n", i, i);
            return;
        }
    }
    va_end(p_args);

    // 尝试分配资源
    va_start(p_args, process_id);
    for (int i = 0; i < bankers_task->num_res; i++)
    {
        arg = va_arg(p_args, int);
        bankers_task->available[i] -= arg;
        bankers_task->process[process_id].allocation[i] += arg;
        bankers_task->process[process_id].need[i] -= arg;
    }
    va_end(p_args);

    // 安全检查
    if (safety_check(bankers_task))
    {
        // 分配成功
        fprintf(stdout, "资源分配成功，安全分配序列为：");
        for (int i = 0; i < bankers_task->num_process; i++)
            fprintf(stdout, "%d ", bankers_task->safe_list[i]);
        fprintf(stdout, "\n");
    }
    else
    {
        // 分配失败，回复原来的分配情况
        va_start(p_args, process_id);
        for (int i = 0; i < bankers_task->num_res; i++)
        {
            arg = va_arg(p_args, int);
            bankers_task->available[i] += arg;
            bankers_task->process[process_id].allocation[i] -= arg;
            bankers_task->process[process_id].need[i] += arg;
        }
        va_end(p_args);
        fprintf(stdout, "资源分配失败\n");
    }
}

int safety_check(BankersTask *bankers_task)
{
    // Work = Available
    memcpy(bankers_task->work, bankers_task->available, sizeof(int) * bankers_task->num_res);

    // Finish = False
    memset(bankers_task->finish, 0, sizeof(int) * bankers_task->num_process);

    int cnt = 0;
    while (1)
    {
        int found = -1; // 标记符合要求的进程ID
        int flag = 1;
        for (int i = 0; i < bankers_task->num_process; i++)
        {
            if (bankers_task->finish[i] == 0)
            {
                flag = 1;
                for (int j = 0; j < bankers_task->num_res; j++)
                {
                    if (bankers_task->process[i].need[j] > bankers_task->work[j])
                    {
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                    found = i;
            }
            if (found != -1)
                break;
        }
        if (found != -1)
        {
            fprintf(stdout, "\t处理进程%d的需求：", found);
            for (int i = 0; i < bankers_task->num_res; i++)
            {
                fprintf(stdout, "%d ", bankers_task->process[found].allocation[i]);
            }
            fprintf(stdout, "\n\t处理前的Work：");
            for (int i = 0; i < bankers_task->num_res; i++)
            {
                fprintf(stdout, "%d ", bankers_task->work[i]);
            }
            for (int i = 0; i < bankers_task->num_res; i++)
            {
                bankers_task->work[i] += bankers_task->process[found].allocation[i];
            }
            fprintf(stdout, "\n\t处理后的Work：");
            for (int i = 0; i < bankers_task->num_res; i++)
            {
                fprintf(stdout, "%d ", bankers_task->work[i]);
            }
            fprintf(stdout, "\n");
            bankers_task->finish[found] = 1;
        }
        else
            break; // 跳出while循环

        // 记录安全分配序列
        bankers_task->safe_list[cnt] = found;
        cnt++;
    }

    // 检查Finish是否全为true
    for (int i = 0; i < bankers_task->num_process; i++)
    {
        if (bankers_task->finish[i] == 0)
        {
            fprintf(stdout, "\t找不到finish=false且Need<=Work的进程\n");
            return 0;
        }
    }
    return 1;
}