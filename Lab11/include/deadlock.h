#ifndef DEADLOCK_H_
#define DEADLOCK_H_

typedef struct Process
{
    int num_res;     // 资源类别数
    int *allocation; // 已分配资源数
    int *request;    // 资源请求数
} Process;

void init_process(Process *process, int num_res);               // 初始化Process对象
void destroy_process(Process *process);                         // 销毁Process对象
void set_process_allocation(Process *process, int *allocation); // 设置allocation成员变量
void set_process_request(Process *process, int *request);       // 设置request成员变量

typedef struct DeadlockTask
{
    int num_process;   // 进程总数
    int num_res;       // 资源类别数
    Process *process;  // Process对象数组
    int *available;    // 系统可用资源数
    int *work;         // 工作向量
    int *set_to_solve; // 未被处理的进程集合。0号位存储集合内进程个数
} DeadlockTask;

void init_deadlock_task(DeadlockTask *deadlock_task, int num_res, int num_process); // 初始化DeadlockTask对象
void destroy_deadlock_task(DeadlockTask *deadlock_task);                            // 销毁DeadlockTask对象
void set_task_process(DeadlockTask *deadlock_task, Process *process);               // 设置process成员变量
void set_task_available(DeadlockTask *deadlock_task, int *available);               // 设置available成员变量
int check_deadlock(DeadlockTask *deadlock_task);                                    // 检测是否发生死锁
void get_set_to_solve(DeadlockTask *deadlock_task);                                 // 获取set_to_solve
void remove_set_item(int *set, int index);                                          // 从集合中删去指定元素，0号元素为集合内元素个数

#endif