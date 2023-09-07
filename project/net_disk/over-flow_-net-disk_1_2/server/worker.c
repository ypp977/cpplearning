#include "threadPool.h"
#include "msg.h"
#include "analysisCmd.h"
#include "checkUser.h"
#include "dir.h"

// 线程函数，用于工作线程处理任务
void *threadFunc(void *arg){
    threadPool_t * pthreadPool = (threadPool_t *)arg; // 将传入的参数转换为线程池结构体指针
    
    while(1){
        // 接收任务
        pthread_mutex_lock(&pthreadPool->taskQueue.mutex); // 加锁，保护任务队列
        while(pthreadPool->taskQueue.queueSize <= 0){ // 如果任务队列为空，则等待任务到来
            pthread_cond_wait(&pthreadPool->taskQueue.cond,&pthreadPool->taskQueue.mutex);
        }
        int netfd = pthreadPool->taskQueue.pFront->netfd; // 取出任务队列头部的套接字描述符
        deQueue(&pthreadPool->taskQueue); // 从任务队列中删除该任务
        pthread_mutex_unlock(&pthreadPool->taskQueue.mutex); // 解锁

        // 接受账号密码
        struct msg_t msg;
        init_msg(&msg);
        if (recv_msg(netfd, &msg) == -1)
        {
            close(netfd);
            printf("One clinet logout or error!\n");
            continue;
        }

        user_t userinfo;
        memcpy(&userinfo, msg.data, msg.size);
        printf("username = %s\n", userinfo.name);        
        // 验证账号密码，失败则断开连接
        if (checkUser(&userinfo, netfd) != 1)
        {
            printf("Error password! One client disconnect!\n");
            close(netfd); // 关闭套接字描述符
            continue;
        }
        printf("Password correct!Start work!\n");

        // 初始化工作目录等
        dirStack_t curDir;
        initDir(&curDir);
        printf("after Init\n");

        while(1)
        {
            // 接受命令
            init_msg(&msg);
            if (recv_msg(netfd, &msg) == -1)
            {
                printf("One client logout or error!\n");
                close(netfd);
                break;
            }
            
            // 解析命令
            if (analysis_cmd(&msg, netfd, &curDir) == -1)
                break;
        }
    }
}

// 创建工作线程
int makeWorker(threadPool_t * pthreadPool){
    for(int i = 0; i < pthreadPool->workerNum; ++i){
        pthread_create(&pthreadPool->tidArr[i],NULL,threadFunc,pthreadPool); // 创建工作线程，并将线程池结构体指针作为参数传入
    }
    return 0;
}

