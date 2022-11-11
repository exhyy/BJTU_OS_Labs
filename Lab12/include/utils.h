#ifndef UTILS_H_
#define UTILS_H_

typedef struct QueueNode
{
    int value;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    int length;
    QueueNode *head;
    QueueNode *tail;
} Queue;

void init_queue(Queue *queue);
void queue_push(Queue *queue, int value);
int queue_pop(Queue *queue);
int queue_top(Queue *queue);
int queue_empty(Queue *queue);

typedef struct Pair 
{
    int first;
    int second;
} Pair;

#endif