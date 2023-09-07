// 防御式声明 避免重复包含
#ifndef __TASK_QUEUE__
#define __TASK_QUEUE__
#include <func.h>

typedef struct task_s {
    int netfd;
    struct task_s * pNext;
} task_t;

typedef struct taskQueue_s {
    task_t * pFront;
    task_t * pRear;
    int queueSize;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} taskQueue_t;

int initQueue(taskQueue_t *pqueue);
int enQueue(taskQueue_t *pqueue, int netfd);
int deQueue(taskQueue_t *pqueue);
#endif

