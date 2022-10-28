#ifndef BANKERS_H_
#define BANKERS_H_

typedef struct Process
{
    int num_res; // 资源类别数
    int *max; // 最大需求
    int *allocation; // 已分配资源数
    int *need; // 尚需资源数
} Process;

void init_process(Process *process, int num_res); // 初始化Process对象
void destroy_process(Process *process); // 销毁Process对象
void set_process_max(Process *process, ...); // 设置max成员变量
void set_process_allocation(Process *process, ...); // 设置allocation成员变量
void set_process_need(Process *process, ...); // 设置need成员变量

typedef struct BankersTask
{
    int num_process; // 进程总数
    int num_res; // 资源类别数
    Process *process; // Process对象数组
    int *available; // 系统可用资源数
    int *work; // 工作向量
    int *finish; // 资源分配完成情况
    int *safe_list; // 安全分配序列
} BankersTask;

void init_bankers_task(BankersTask *bankers_task, int num_res, int num_process); // 初始化BankersTask对象
void destroy_bankers_task(BankersTask *bankers_task); // 销毁BankersTask对象
void set_task_process(BankersTask *bankers_task, ...); // 设置process成员变量
void set_task_available(BankersTask *bankers_task, ...); // 设置available成员变量
void resource_request(BankersTask *bankers_task, int process_id, ...); // 尝试处理资源请求
int safety_check(BankersTask *bankers_task); // 安全性检查

#endif