#ifndef PAGE_H_
#define PAGE_H_

#define PAGE_READ_ONLY 0
#define PAGE_READ_WRITE 1
#define PAGE_NOT_ACCESSED 0
#define PAGE_ACCESSED 1
#define PAGE_NOT_MODIFIED 0
#define PAGE_MODIFIED 1

typedef struct Page
{
    int id;       // 页面号
    int mode;     // 读写权限
    int accessed; // 访问位
    int modified; // 修改位
} Page;

#define ACCESS_READ 1
#define ACCESS_WRITE 2

typedef struct AccessList
{
    int length;
    int *page_id;
    int *access_mode;
} AccessList;

void init_access_list(AccessList *access_list, int length);

typedef struct WorkingSet
{
    int num_pages;          // 工作集页面数
    int start;              // 起始页号
    int speed;              // 移动速率
    AccessList access_list; // 访问序列
} WorkingSet;

void init_working_set(WorkingSet *working_set, int num_pages, int start, int speed, AccessList access_list);

typedef struct Memory
{
    int num_pages;          // 总页面数
    Page *page;             // 所有页面
    WorkingSet working_set; // 工作集
    int missing_cnt;        // 缺页次数
    int access_cnt;         // 访问次数
} Memory;

void init_memory(Memory *memory, int num_pages, WorkingSet working_set);
void get_access_list(Memory *memory);
void print_loaded(int loaded[]);
int match_page(int loaded[], int page_id);

void OPT(Memory *memory);           // 最佳置换算法
void FIFO(Memory *memory);          // 先进先出置换算法
void LRU(Memory *memory);           // 最近最久未使用置换算法
void simple_clock(Memory *memory);  // 简单Clock算法
void refined_clock(Memory *memory); // 改进型Clock算法

#endif