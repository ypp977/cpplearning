#ifndef __THREAD_POOL__ // 如果未定义 __THREAD_POOL__ 宏，则进行下面的代码编译
#define __THREAD_POOL__ // 定义 __THREAD_POOL__ 宏

#include "taskQueue.h"
#include "msg.h"
#include "dir.h"
#include "createUser.h"
#include "userLogin.h"
#include "getCryptPasswd.h"
#include "s_file.h"
#include "log.h"

typedef struct threadPool_s { // 定义线程池结构体
    pthread_t *tidArr; // 工作线程ID数组
    int workerNum; // 工作线程数量
    taskQueue_t taskQueue; // 任务队列
} threadPool_t;

int initThreadPool(threadPool_t * pthreadPool, int workerNum); // 初始化线程池函数声明
int tcpInit(char *ip, char *port,int *psockfd); // TCP初始化函数声明
void *threadFunc(void *arg); // 线程函数声明
int makeWorker(threadPool_t * pthreadPool); // 创建工作线程函数声明
int epollAdd(int epfd, int fd); // 添加事件到epoll实例函数声明
int epollDel(int epfd, int fd); // 从epoll实例中删除事件函数声明
                                
#endif


