#include "bankers.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

int main()
{
    printf("20281109-HuangYuyao-OS_Lab10\n");

    Process p[5];
    for (int i = 0; i < 5; i++)
        init_process(&p[i], 3);

    // 设置进程0
    set_process_max(&p[0], 7, 5, 3);
    set_process_allocation(&p[0], 0, 1, 0);
    set_process_need(&p[0], 7, 4, 3);

    // 设置进程1
    set_process_max(&p[1], 3, 2, 2);
    set_process_allocation(&p[1], 2, 0, 0);
    set_process_need(&p[1], 1, 2, 2);

    // 设置进程2
    set_process_max(&p[2], 9, 0, 2);
    set_process_allocation(&p[2], 3, 0, 2);
    set_process_need(&p[2], 6, 0, 0);

    // 设置进程3
    set_process_max(&p[3], 2, 2, 2);
    set_process_allocation(&p[3], 2, 1, 1);
    set_process_need(&p[3], 0, 1, 1);

    // 设置进程4
    set_process_max(&p[4], 4, 3, 3);
    set_process_allocation(&p[4], 0, 0, 2);
    set_process_need(&p[4], 4, 3, 1);

    // 设置银行家算法模型任务
    BankersTask task;
    init_bankers_task(&task, 3, 5);
    set_task_process(&task, p[0], p[1], p[2], p[3], p[4]);
    set_task_available(&task, 3, 3, 2);

    resource_request(&task, 0, 0, 0, 0); // T0时刻
    printf("--------------\n");
    resource_request(&task, 1, 1, 0, 2); // T1时刻
    printf("--------------\n");
    resource_request(&task, 4, 3, 3, 0); // T2时刻
    printf("--------------\n");
    resource_request(&task, 0, 0, 2, 0); // T3时刻
    printf("--------------\n");
    resource_request(&task, 0, 0, 1, 0); // T3时刻

    return 0;
}