#include "FreeTable.h"
#include <malloc.h>
#include <string.h>

void init_free_table(FreeTable *free_table, int size)
{
    free_table->size = size;
    free_table->length = 0;
    free_table->data = (FreeTableItem *)malloc(size * sizeof(FreeTableItem));
}

void destory_free_table(FreeTable *free_table)
{
    free_table->size = 0;
    free_table->length = 0;
    free(free_table->data);
}

void set_free_table_item(FreeTable *free_table, FreeTableItem free_table_item, int index, int new_item)
{
    free_table->data[index] = free_table_item;
    if (new_item)
        free_table->length++;
}

void insert_free_table_item(FreeTable *free_table, FreeTableItem free_table_item, int index)
{
    if (free_table->length == free_table->size)
    {
        FreeTableItem *old_data = free_table->data;
        free_table->data = (FreeTableItem *)malloc(free_table->size * 2 * sizeof(FreeTableItem));
        memcpy(free_table->data, old_data, free_table->size * sizeof(FreeTableItem));
        free_table->size *= 2;
        free(old_data);
    }

    for (int i = free_table->length - 1; i > index; i--)
    {
        free_table->data[i + 1] = free_table->data[i];
    }
    free_table->data[index + 1] = free_table_item;
    free_table->length++;
}

void remove_free_table_item(FreeTable *free_table, int index)
{
    for (int i = index; i < free_table->length; i++)
    {
        free_table->data[i] = free_table->data[i + 1];
    }
    free_table->length--;
}

void print_free_table(FreeTable *free_table)
{
    fprintf(stdout, "%-8s%-12s%-12s%-8s\n", "ID", "Size(KB)", "Address(K)", "Status");
    for (int i = 0; i < free_table->length; i++)
    {
        FreeTableItem item = free_table->data[i];
        if (item.status == PARTITION_FREE)
            fprintf(stdout, "%-8d%-12.2f%-12d%-8s\n", i, item.size / 1024.0, item.address / 1024, "FREE");
        else
            fprintf(stdout, "%-8d%-12.2f%-12d%-8s\n", i, item.size / 1024.0, item.address / 1024, "USED");
    }
}

double get_memory_usage(FreeTable *free_table)
{
    double used_size = 0, total_size = 0;
    for (int i = 0; i < free_table->length; i++)
    {
        total_size += free_table->data[i].size;
        if (free_table->data[i].status == PARTITION_USED)
            used_size += free_table->data[i].size;
    }
    return used_size / total_size;
}