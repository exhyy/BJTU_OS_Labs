#include "utils.h"
#include <stdlib.h>
#include <malloc.h>

void init_queue(Queue *queue)
{
    queue->length = 0;
    queue->head = NULL;
    queue->tail = NULL;
}

void queue_push(Queue *queue, int value)
{
    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    node->value = value;
    node->next = NULL;
    if (queue->head == NULL)
    {
        queue->head = node;
        queue->tail = node;
    }
    else
    {
        queue->tail->next = node;
        queue->tail = node;
    }
}

int queue_pop(Queue *queue)
{
    int head_value = queue_head(queue);
    QueueNode *old_head = queue->head;
    if (queue->head == queue->tail)
    {
        queue->head = NULL;
        queue->tail = NULL;
    }
    else
        queue->head = queue->head->next;
    free(old_head);
    return head_value;
}

int queue_head(Queue *queue)
{
    return queue->head->value;
}

int queue_empty(Queue *queue)
{
    return queue->head == NULL;
}