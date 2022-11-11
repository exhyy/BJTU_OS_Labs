#ifndef EVENT_H_
#define EVENT_H_

#define EMPTY 0
#define CREATE_PROCESS 1
#define SUSPEND_PROCESS 2
#define ACTIVATE_PROCESS 3

typedef struct Event
{
    int type;
    int start_time;
    int duration;
} Event;

#endif