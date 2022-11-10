#include "FreeTable.h"
#include <malloc.h>

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

void add_free_table_item(FreeTable *free_table, FreeTableItem free_table_item, int index)
{
    
}