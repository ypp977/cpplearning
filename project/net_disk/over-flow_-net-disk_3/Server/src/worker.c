#include "../Include/analysisCmd.h"
#define HOST "localhost"        // 连接本地数据库
#define USER "root"             // 连接数据库的用户
#define PASSWD "88888888"       // 连接数据库的用户密码
#define DB "overflow_netdisk"   // 数据库名

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
        MYSQL* mysql = mysql_init(NULL); // 初始化数据库
        pthread_mutex_unlock(&pthreadPool->taskQueue.mutex); // 解锁

        // 一些初始化工作
        struct msg_t msg;
        MYSQL* ret = mysql_real_connect( mysql, HOST, USER, PASSWD, DB, 0, NULL, 0);
        if (ret == NULL)
        {
            fprintf(stderr, "mysql_real_connect: %s\n", mysql_error(mysql));
            exit(1);
        }

        userCwd_t userCwd;
        user_t userInfo;
        // 循环处理命令
        while(1)
        {
            // 重置消息
            init_msg(&msg);
            // 接收消息，出错退出
            if (recv_msg(netfd, &msg) == -1)
            {
                printf("One client logout or error!\n");
                close(netfd);
                break;
            }
            // 解析命令，返回-1表示用户主动quit
            if (analysis_cmd(&msg, netfd, mysql, &userCwd, &userInfo) == -1)
                break;
        }
        // closemysql
        mysql_close(mysql);
    }
}

// 创建工作线程
int makeWorker(threadPool_t * pthreadPool){
    for(int i = 0; i < pthreadPool->workerNum; ++i){
        pthread_create(&pthreadPool->tidArr[i],NULL,threadFunc,pthreadPool); // 创建工作线程，并将线程池结构体指针作为参数传入
    }
    return 0;
}


