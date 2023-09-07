#include "../Include/head.h" // 引入自定义的线程池头文件

// 初始化线程池
int initThreadPool(threadPool_t * pthreadPool, int workerNum)
{
    pthreadPool->workerNum = workerNum; // 设置工作线程数量
    pthreadPool->tidArr = (pthread_t *)calloc(pthreadPool->workerNum,sizeof(pthread_t)); // 为线程ID数组分配内存
    initQueue(&pthreadPool->taskQueue); // 初始化任务队列
    return 0;
}

int main(int argc, char *argv[])
{
    // read config
    FILE* fp = fopen("server.conf", "r");
    char ip[256];
    char port[256];
    char threadNum[256];

    fscanf(fp, "%s %s %s", ip, port, threadNum);
    fclose(fp);

    threadPool_t threadPool; // 定义线程池结构体
    int workerNum = atoi(threadNum); // 将第三个命令行参数转换为整型，表示工作线程数量
    initThreadPool(&threadPool,workerNum); // 初始化线程池

    int sockfd; // 定义套接字描述符
    tcpInit(ip,port,&sockfd); // 初始化TCP连接，将服务器IP和端口号作为参数传入，并将返回的套接字描述符保存在sockfd中

    makeWorker(&threadPool); // 创建工作线程

    int epfd = epoll_create(1); // 创建epoll实例
    epollAdd(epfd,sockfd); // 将sockfd添加到epoll实例中进行监听
    struct epoll_event readyset[1024]; // 定义epoll事件数组，用于保存就绪的事件

    while(1)
    {
        int readynum = epoll_wait(epfd,readyset,1024,-1); // 等待事件发生，返回就绪的事件数量
        for(int i = 0; i < readynum; ++i)
        {
            if(readyset[i].data.fd == sockfd)
            {
                // 如果有新的客户端连接
                int netfd = accept(sockfd,NULL,NULL); // 接受连接请求，获取与客户端通信的套接字描述符
                printf("One client connected!\n");
                pthread_mutex_lock(&threadPool.taskQueue.mutex); // 加锁，保护任务队列
                enQueue(&threadPool.taskQueue,netfd); // 将套接字描述符加入任务队列
                pthread_cond_signal(&threadPool.taskQueue.cond); // 发送信号，通知工作线程有任务可执行
                pthread_mutex_unlock(&threadPool.taskQueue.mutex); // 解锁
            }
        }
    }
}

