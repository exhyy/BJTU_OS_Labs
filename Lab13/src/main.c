#include "page.h"
#include <stdio.h>
#include <stdlib.h>

#define RANDOM_SEED 114514

int main()
{
    printf("20281109-HuangYuyao-OS_Lab13\n");

    srand(RANDOM_SEED);

    int access_list_length, num_pages;
    int start, speed, working_set_length;

    fprintf(stdout, "现在需要构建随机的访问序列！\n");
    fprintf(stdout, "请输入访问序列的长度：");
    fscanf(stdin, "%d", &access_list_length);
    fprintf(stdout, "请输入页面总数N：");
    fscanf(stdin, "%d", &num_pages);
    fprintf(stdout, "请输入工作集起始页号s：");
    fscanf(stdin, "%d", &start);
    fprintf(stdout, "请输入工作集包含的页面数w：");
    fscanf(stdin, "%d", &working_set_length);
    fprintf(stdout, "请输入工作集移动速率v：");
    fscanf(stdin, "%d", &speed);

    AccessList access_list;
    WorkingSet working_set;
    Memory memory;
    init_access_list(&access_list, access_list_length);
    init_working_set(&working_set, working_set_length, start, speed, access_list);
    init_memory(&memory, num_pages, working_set);
    get_access_list(&memory);

    fprintf(stdout, "生成的访问序列如下（R表示读访问，W表示写访问）：\n");
    for (int i = 0; i < memory.working_set.access_list.length; i++)
    {
        if (memory.working_set.access_list.access_mode[i] == ACCESS_READ)
            fprintf(stdout, "%d(R) ", memory.working_set.access_list.page_id[i]);
        else
            fprintf(stdout, "%d(W) ", memory.working_set.access_list.page_id[i]);
    }
    fprintf(stdout, "\n");

    int algo;
    fprintf(stdout, "支持的页面淘汰算法如下：\n");
    fprintf(stdout, "  1. 最佳淘汰算法\n");
    fprintf(stdout, "  2. 先进先出淘汰算法\n");
    fprintf(stdout, "  3. 最近最久未使用淘汰算法\n");
    fprintf(stdout, "  4. 简单Clock淘汰算法\n");
    fprintf(stdout, "  5. 改进型Clock淘汰算法\n");
    fprintf(stdout, "请输入页面淘汰算法编号：");
    fscanf(stdin, "%d", &algo);
    switch (algo)
    {
    case 1:
        OPT(&memory);
        break;
    
    case 2:
        FIFO(&memory);
        break;

    case 3:
        LRU(&memory);
        break;

    case 4:
        simple_clock(&memory);
        break;

    case 5:
        refined_clock(&memory);
        break;

    default:
        fprintf(stderr, "错误：未知页面淘汰算法\n");
        break;
    }
    return 0;
}