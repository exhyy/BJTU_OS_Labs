#include "deadlock.h"
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void init_process(Process *process, int num_res)
{
    process->num_res = num_res;
    process->allocation = (int *)malloc(num_res * sizeof(int));
    process->request = (int *)malloc(num_res * sizeof(int));
}

void destroy_process(Process *process)
{
    process->num_res = -1;
    free(process->allocation);
    free(process->request);
}

void set_process_allocation(Process *process, int *allocation)
{
    for (int i = 0; i < process->num_res; i++)
    {
        process->allocation[i] = allocation[i];
    }
}

void set_process_request(Process *process, int *request)
{
    for (int i = 0; i < process->num_res; i++)
    {
        process->request[i] = request[i];
    }
}

void init_deadlock_task(DeadlockTask *deadlock_task, int num_res, int num_process)
{
    deadlock_task->num_res = num_res;
    deadlock_task->num_process = num_process;
    deadlock_task->process = (Process *)malloc(num_process * sizeof(Process));
    deadlock_task->available = (int *)malloc(num_res * sizeof(int));
    deadlock_task->work = (int *)malloc(num_res * sizeof(int));
    deadlock_task->set_to_solve = (int *)malloc((num_process + 1) * sizeof(int));
}

void destroy_deadlock_task(DeadlockTask *deadlock_task)
{
    deadlock_task->num_process = -1;
    deadlock_task->num_res = -1;
    free(deadlock_task->process);
    free(deadlock_task->available);
    free(deadlock_task->work);
    free(deadlock_task->set_to_solve);
}

void set_task_process(DeadlockTask *deadlock_task, Process *process)
{
    for (int i = 0; i < deadlock_task->num_process; i++)
    {
        deadlock_task->process[i] = process[i];
    }
}

void set_task_available(DeadlockTask *deadlock_task, int *available)
{
    for (int i = 0; i < deadlock_task->num_res; i++)
    {
        deadlock_task->available[i] = available[i];
    }
}

int check_deadlock(DeadlockTask *deadlock_task)
{
    int *temp_set = (int *)malloc((deadlock_task->num_process + 1) * sizeof(int));
    get_set_to_solve(deadlock_task);
    memcpy(deadlock_task->work, deadlock_task->available, deadlock_task->num_res * sizeof(int));
    memcpy(temp_set, deadlock_task->set_to_solve, (deadlock_task->num_process + 1) * sizeof(int));

    while (deadlock_task->set_to_solve[0] != 0)
    {
        for (int i = 1; i <= deadlock_task->set_to_solve[0]; i++)
        {
            int flag = 1;
            Process process = deadlock_task->process[deadlock_task->set_to_solve[i]];
            for (int j = 0; j < deadlock_task->num_res; j++)
            {
                if (process.request[j] > deadlock_task->work[j])
                {
                    flag = 0;
                    break;
                }
            }

            if (flag)
            {
                // Request[i] <= Work
                for (int j = 0; j < deadlock_task->num_res; j++)
                {
                    deadlock_task->work[j] += process.allocation[j];
                }
                remove_set_item(deadlock_task->set_to_solve, i);
            }
        }
        if (deadlock_task->set_to_solve[0] == temp_set[0])
            break;
        else
            memcpy(temp_set, deadlock_task->set_to_solve, (deadlock_task->num_process + 1) * sizeof(int));
    }

    free(temp_set);
    return deadlock_task->set_to_solve[0] != 0;
}

void get_set_to_solve(DeadlockTask *deadlock_task)
{
    deadlock_task->set_to_solve[0] = 0;
    for (int i = 0; i < deadlock_task->num_process; i++)
    {
        Process process = deadlock_task->process[i];
        for (int j = 0; j < deadlock_task->num_res; j++)
        {
            if (process.allocation[j] != 0 || process.request[j] != 0)
            {
                deadlock_task->set_to_solve[0]++;
                deadlock_task->set_to_solve[deadlock_task->set_to_solve[0]] = i;
                break;
            }
        }
    }
}

void remove_set_item(int *set, int index)
{
    if (index == 0)
        return;
    for (int i = index; i <= set[0]; i++)
    {
        set[i] = set[i + 1];
    }
    set[0]--;
}