#ifndef FREETABLE_H_
#define FREETABLE_H_

// status
#define PARTITION_FREE 0
#define PARTITION_USED 1

typedef struct FreeTableItem
{
    int size;
    int address;
    int status;
} FreeTableItem;

typedef struct FreeTable
{
    int size;   // 占用的大小
    int length; // 实际使用的大小
    FreeTableItem *data;
} FreeTable;

void init_free_table(FreeTable *free_table, int size);
void destory_free_table(FreeTable *free_table);
void set_free_table_item(FreeTable *free_table, FreeTableItem free_table_item, int index, int new_item);
void insert_free_table_item(FreeTable *free_table, FreeTableItem free_table_item, int index);
void remove_free_table_item(FreeTable *free_table, int index);
void print_free_table(FreeTable *free_table);
double get_memory_usage(FreeTable *free_table);

#endif