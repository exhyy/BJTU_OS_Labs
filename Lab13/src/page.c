#include "page.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void init_access_list(AccessList *access_list, int length)
{
    access_list->length = length;
    access_list->page_id = (int *)malloc(length * sizeof(int));
    access_list->access_mode = (int *)malloc(length * sizeof(int));
}

void init_working_set(WorkingSet *working_set, int num_pages, int start, int speed, AccessList access_list)
{
    working_set->num_pages = num_pages;
    working_set->start = start;
    working_set->speed = speed;
    working_set->access_list = access_list;
}

void init_memory(Memory *memory, int num_pages, WorkingSet working_set)
{
    memory->num_pages = num_pages;
    memory->page = (Page *)malloc(num_pages * sizeof(Page));
    memory->missing_cnt = 0;
    memory->access_cnt = 0;
    for (int i = 0; i < num_pages; i++)
    {
        memory->page->id = -1; // -1表示这一项不使用
        // 70%页面支持读写，30%页面只读
        double random_num = (double)rand() / RAND_MAX;
        if (random_num > 0.7)
        {
            memory->page[i].mode = PAGE_READ_ONLY;
        }
        else
        {
            memory->page[i].mode = PAGE_READ_WRITE;
        }
    }
    memory->working_set = working_set;
}

void get_access_list(Memory *memory)
{
    int i = 0;
    while (i < memory->working_set.access_list.length)
    {
        // 生成[s, min(s+w, N-1)]的v个随机数，保存到页面访问序列中
        for (int j = 0; j < memory->working_set.speed && i < memory->working_set.access_list.length; j++)
        {
            int random_min = memory->working_set.start;
            int random_max;
            if (memory->working_set.start + memory->working_set.num_pages > memory->num_pages - 1)
                random_max = memory->num_pages - 1;
            else
                random_max = memory->working_set.start + memory->working_set.num_pages;
            int page_id = rand() % (random_max - random_min + 1) + random_min;
            memory->working_set.access_list.page_id[i] = page_id;

            // 确定读写方式
            double r = (double)rand() / RAND_MAX;
            if (r > 0.7 && memory->page[page_id].mode == PAGE_READ_WRITE)
                memory->working_set.access_list.access_mode[i] = ACCESS_WRITE;
            else
                memory->working_set.access_list.access_mode[i] = ACCESS_READ;
            i++;
        }

        // 工作集移动，20%概率重新生成start
        double r = (double)rand() / RAND_MAX;
        if (r < 0.2)
            memory->working_set.start = rand() % memory->num_pages;
        else
            memory->working_set.start = (memory->working_set.start + 1) % memory->num_pages;
    }
}

void print_loaded(int loaded[])
{
    fprintf(stdout, "[");
    for (int i = 0; i < 3; i++)
    {
        if (loaded[i] == -1)
            fprintf(stdout, "空");
        else
            fprintf(stdout, "%d", loaded[i]);
        if (i != 2)
            fprintf(stdout, ", "); // 分隔符
    }
    fprintf(stdout, "]");
}

int match_page(int loaded[], int page_id)
{
    for (int i = 0; i < 3; i++)
        if (loaded[i] == page_id)
            return i; // 匹配返回对应的下标
    return -1;        // 不匹配返回-1
}

void OPT(Memory *memory)
{
    int loaded[3] = {-1, -1, -1}; // 3个被装在到内存的页面号
    AccessList access_list = memory->working_set.access_list;
    int preloaded_cnt = 0;
    double total_time = 0;
    clock_t start_time, end_time;

    for (int i = 0; i < access_list.length; i++)
    {
        if (access_list.access_mode[i] == ACCESS_READ)
            fprintf(stdout, "读页面%d...", access_list.page_id[i]);
        else if (access_list.access_mode[i] == ACCESS_WRITE)
            fprintf(stdout, "写页面%d...", access_list.page_id[i]);
        else
            fprintf(stderr, "错误：未知内存访问方式\n");

        if (preloaded_cnt < 3)
        {
            int flag = 1; // 标记是否缺页
            for (int j = 0; j <= preloaded_cnt - 1; j++)
            {
                if (loaded[j] == access_list.page_id[i])
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
            {
                loaded[preloaded_cnt] = access_list.page_id[i];
                preloaded_cnt++;
            }
            memory->access_cnt++;
            print_loaded(loaded);
            fprintf(stdout, "...预装入阶段不统计缺页次数\n");
            continue;
        }

        if (match_page(loaded, access_list.page_id[i]) != -1)
        {
            print_loaded(loaded);
            memory->access_cnt++;
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...未发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
        else
        {
            start_time = clock();
            int earliest[3];
            memset(earliest, 0x3f, sizeof(earliest)); // 设置为正无穷
            int replace = -1;
            for (int j = 0; j < 3; j++)
            {
                for (int k = i; k < access_list.length; k++)
                {
                    if (access_list.page_id[k] == loaded[j])
                    {
                        earliest[j] = k; // 内存中第j个页面最早在第k次访问时被调入
                        break;
                    }
                }
            }
            if (earliest[0] >= earliest[1] && earliest[0] >= earliest[2])
                replace = 0;
            else if (earliest[1] >= earliest[0] && earliest[1] >= earliest[2])
                replace = 1;
            else
                replace = 2;
            loaded[replace] = access_list.page_id[i];
            memory->missing_cnt++;
            memory->access_cnt++;
            end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
            print_loaded(loaded);
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
    }
    fprintf(stdout, "总耗时：%lfs\n", total_time);
}

void FIFO(Memory *memory)
{
    int loaded[3] = {-1, -1, -1}; // 3个被装在到内存的页面号
    AccessList access_list = memory->working_set.access_list;
    int preloaded_cnt = 0;
    double total_time = 0;
    clock_t start_time, end_time;

    // 未被访问次数
    int not_access[3] = {0, 0, 0};

    for (int i = 0; i < access_list.length; i++)
    {
        if (access_list.access_mode[i] == ACCESS_READ)
            fprintf(stdout, "读页面%d...", access_list.page_id[i]);
        else if (access_list.access_mode[i] == ACCESS_WRITE)
            fprintf(stdout, "写页面%d...", access_list.page_id[i]);
        else
            fprintf(stderr, "错误：未知内存访问方式\n");

        // 所有页面的未访问次数+1
        for (int j = 0; j < 3; j++)
            not_access[j]++;

        if (preloaded_cnt < 3)
        {
            int flag = 1; // 标记是否缺页
            for (int j = 0; j <= preloaded_cnt - 1; j++)
            {
                if (loaded[j] == access_list.page_id[i])
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
            {
                loaded[preloaded_cnt] = access_list.page_id[i];
                preloaded_cnt++;
            }
            not_access[preloaded_cnt - 1] = 0; // 页面被访问，次数置0
            memory->access_cnt++;
            print_loaded(loaded);
            fprintf(stdout, "...预装入阶段不统计缺页次数\n");
            continue;
        }

        int match_index = match_page(loaded, access_list.page_id[i]);
        if (match_index != -1)
        {
            print_loaded(loaded);
            memory->access_cnt++;
            // not_access[match_index] = 0;
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...未发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
        else
        {
            start_time = clock();
            int max_index = -1;
            int max_value = -1;
            for (int j = 0; j < 3; j++)
            {
                if (not_access[j] >= max_value)
                {
                    max_value = not_access[j];
                    max_index = j;
                }
            }
            loaded[max_index] = access_list.page_id[i];
            memory->missing_cnt++;
            memory->access_cnt++;
            not_access[max_index] = 0;
            end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
            print_loaded(loaded);
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
    }
    fprintf(stdout, "总耗时：%lfs\n", total_time);
}

void LRU(Memory *memory)
{
    int loaded[3] = {-1, -1, -1}; // 3个被装在到内存的页面号
    AccessList access_list = memory->working_set.access_list;
    int preloaded_cnt = 0;
    double total_time = 0;
    clock_t start_time, end_time;

    // 未被访问次数
    int not_access[3] = {0, 0, 0};

    for (int i = 0; i < access_list.length; i++)
    {
        if (access_list.access_mode[i] == ACCESS_READ)
            fprintf(stdout, "读页面%d...", access_list.page_id[i]);
        else if (access_list.access_mode[i] == ACCESS_WRITE)
            fprintf(stdout, "写页面%d...", access_list.page_id[i]);
        else
            fprintf(stderr, "错误：未知内存访问方式\n");

        // 所有页面的未访问次数+1
        for (int j = 0; j < 3; j++)
            not_access[j]++;

        if (preloaded_cnt < 3)
        {
            int flag = 1; // 标记是否缺页
            for (int j = 0; j <= preloaded_cnt - 1; j++)
            {
                if (loaded[j] == access_list.page_id[i])
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
            {
                loaded[preloaded_cnt] = access_list.page_id[i];
                preloaded_cnt++;
            }
            not_access[preloaded_cnt - 1] = 0; // 页面被访问，次数置0
            memory->access_cnt++;
            print_loaded(loaded);
            fprintf(stdout, "...预装入阶段不统计缺页次数\n");
            continue;
        }

        int match_index = match_page(loaded, access_list.page_id[i]);
        if (match_index != -1)
        {
            print_loaded(loaded);
            memory->access_cnt++;
            not_access[match_index] = 0;
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...未发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
        else
        {
            start_time = clock();
            int max_index = -1;
            int max_value = -1;
            for (int j = 0; j < 3; j++)
            {
                if (not_access[j] >= max_value)
                {
                    max_value = not_access[j];
                    max_index = j;
                }
            }
            loaded[max_index] = access_list.page_id[i];
            memory->missing_cnt++;
            memory->access_cnt++;
            not_access[max_index] = 0;
            end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
            print_loaded(loaded);
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
    }
    fprintf(stdout, "总耗时：%lfs\n", total_time);
}

void simple_clock(Memory *memory)
{
    Page loaded[3] = {{-1}, {-1}, {-1}}; // 3个被装在到内存的页面号和访问标志位
    int replace_index = 0;
    AccessList access_list = memory->working_set.access_list;
    int preloaded_cnt = 0;
    double total_time = 0;
    clock_t start_time, end_time;

    for (int i = 0; i < access_list.length; i++)
    {
        int loaded_t[3];
        if (access_list.access_mode[i] == ACCESS_READ)
            fprintf(stdout, "读页面%d...", access_list.page_id[i]);
        else if (access_list.access_mode[i] == ACCESS_WRITE)
            fprintf(stdout, "写页面%d...", access_list.page_id[i]);
        else
            fprintf(stderr, "错误：未知内存访问方式\n");

        if (preloaded_cnt < 3)
        {
            int flag = 1; // 标记是否缺页
            for (int j = 0; j <= preloaded_cnt - 1; j++)
            {
                if (loaded[j].id == access_list.page_id[i])
                {
                    flag = 0;
                    break;
                }
            }
            if (flag == 1)
            {
                loaded[preloaded_cnt].id = access_list.page_id[i];
                loaded[preloaded_cnt].accessed = PAGE_ACCESSED;
                preloaded_cnt++;
            }
            memory->access_cnt++;
            for (int j = 0; j < 3; j++)
                loaded_t[j] = loaded[j].id;
            print_loaded(loaded_t);
            fprintf(stdout, "...预装入阶段不统计缺页次数\n");
            continue;
        }

        for (int j = 0; j < 3; j++)
            loaded_t[j] = loaded[j].id;
        if (match_page(loaded_t, access_list.page_id[i]) != -1)
        {
            print_loaded(loaded_t);
            memory->access_cnt++;
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...未发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
        else
        {
            start_time = clock();
            int flag = 1;
            while (flag)
            {
                if (loaded[replace_index].accessed == PAGE_ACCESSED)
                    loaded[replace_index].accessed = PAGE_NOT_ACCESSED;
                else
                {
                    loaded[replace_index].id = access_list.page_id[i];
                    loaded[replace_index].accessed = PAGE_ACCESSED;
                    flag = 0;
                }
                replace_index = (replace_index + 1) % 3;
            }
            memory->missing_cnt++;
            memory->access_cnt++;
            end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
            for (int j = 0; j < 3; j++)
                loaded_t[j] = loaded[j].id;
            print_loaded(loaded_t);
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
    }
    fprintf(stdout, "总耗时：%lfs\n", total_time);
}

void refined_clock(Memory *memory)
{
    Page loaded[3] = {{-1}, {-1}, {-1}}; // 3个被装在到内存的页面号和访问标志位
    int replace_index = 0;
    AccessList access_list = memory->working_set.access_list;
    int preloaded_cnt = 0;
    double total_time = 0;
    clock_t start_time, end_time;

    for (int i = 0; i < access_list.length; i++)
    {
        int loaded_t[3];
        if (access_list.access_mode[i] == ACCESS_READ)
            fprintf(stdout, "读页面%d...", access_list.page_id[i]);
        else if (access_list.access_mode[i] == ACCESS_WRITE)
            fprintf(stdout, "写页面%d...", access_list.page_id[i]);
        else
            fprintf(stderr, "错误：未知内存访问方式\n");

        if (preloaded_cnt < 3)
        {
            int flag = 1; // 标记是否缺页
            for (int j = 0; j <= preloaded_cnt - 1; j++)
            {
                if (loaded[j].id == access_list.page_id[i])
                {
                    flag = 0;
                    if (access_list.access_mode[i] == ACCESS_WRITE)
                        loaded[j].modified = PAGE_MODIFIED;
                    break;
                }
            }
            if (flag == 1)
            {
                loaded[preloaded_cnt].id = access_list.page_id[i];
                loaded[preloaded_cnt].accessed = PAGE_ACCESSED;
                if (access_list.access_mode[i] == ACCESS_WRITE)
                    loaded[preloaded_cnt].modified = PAGE_MODIFIED;
                else
                    loaded[preloaded_cnt].modified = PAGE_NOT_MODIFIED;
                preloaded_cnt++;
            }
            memory->access_cnt++;
            for (int j = 0; j < 3; j++)
                loaded_t[j] = loaded[j].id;
            print_loaded(loaded_t);
            fprintf(stdout, "...预装入阶段不统计缺页次数\n");
            continue;
        }

        for (int j = 0; j < 3; j++)
            loaded_t[j] = loaded[j].id;
        int match_index = match_page(loaded_t, access_list.page_id[i]);
        if (match_index != -1)
        {
            if (access_list.access_mode[i] == ACCESS_WRITE)
                loaded[match_index].modified = PAGE_MODIFIED;
            print_loaded(loaded_t);
            memory->access_cnt++;
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...未发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
        else
        {
            start_time = clock();
            int loop_cnt = 1;
            while (1)
            {
                int index = replace_index;
                if (loop_cnt % 2 == 1)
                {
                    int flag = 0;
                    while (1)
                    {
                        if (loaded[index].accessed == PAGE_NOT_ACCESSED && loaded[index].modified == PAGE_NOT_MODIFIED)
                        {
                            flag = 1;
                            break;
                        }
                        index = (index + 1) % 3;
                        if (index == replace_index)
                            break;
                    }
                    if (flag)
                    {
                        replace_index = index;
                        break;
                    }
                }
                else
                {
                    int flag = 0;
                    while (1)
                    {
                        if (loaded[index].accessed == PAGE_NOT_ACCESSED && loaded[index].modified == PAGE_MODIFIED)
                        {
                            flag = 1;
                            break;
                        }
                        loaded[index].accessed = PAGE_NOT_ACCESSED;
                        index = (index + 1) % 3;
                        if (index == replace_index)
                            break;
                    }
                    if (flag)
                    {
                        replace_index = index;
                        break;
                    }
                }
                loop_cnt++;
            }
            loaded[replace_index].id = access_list.page_id[i];
            loaded[replace_index].accessed = PAGE_ACCESSED;
            if (access_list.access_mode[i] == ACCESS_WRITE)
                loaded[replace_index].modified = PAGE_MODIFIED;
            else
                loaded[replace_index].modified = PAGE_NOT_MODIFIED;
            memory->missing_cnt++;
            memory->access_cnt++;
            end_time = clock();
            total_time += (double)(end_time - start_time) / CLOCKS_PER_SEC;
            for (int j = 0; j < 3; j++)
                loaded_t[j] = loaded[j].id;
            print_loaded(loaded_t);
            double rate = (double)(memory->missing_cnt) / memory->access_cnt;
            fprintf(stdout, "...发生缺页中断，缺页率为%.2lf%%(%d/%d)\n", rate * 100, memory->missing_cnt, memory->access_cnt);
        }
    }
    fprintf(stdout, "总耗时：%lfs\n", total_time);
}