// 防御式声明 避免重复包含
#ifndef __TASK_QUEUE__
#define __TASK_QUEUE__
#include <func.h>
#include "msg.h"
#include "dir.h"
#define BUFSIZE 4096

typedef struct task_s {
    int netfd;//连接口
    int flag;//1为接收文件 2为传文件
    char buf[BUFSIZE];//存放参数
    struct task_s * pNext;
} task_t;

typedef struct taskQueue_s {
    task_t * pFront;
    task_t * pRear;
    int queueSize;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    userCwd_t netfd2Usr[4096];
} taskQueue_t;

typedef struct mysqlMutex_s{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}mysqlMutex_t;
int initQueue(taskQueue_t *pqueue);
int enQueue(taskQueue_t *pqueue, int netfd,int flag, char* path);
int deQueue(taskQueue_t *pqueue);
#endif

