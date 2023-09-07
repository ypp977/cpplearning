#include "../Include/analysisCmd.h"
#include "../Include/taskQueue.h"
#include "../Include/user_mng.h"

// 初始化线程池
int initThreadPool(threadPool_t * pthreadPool, int workerNum, MYSQL* mysql)
{
    pthreadPool->workerNum = workerNum; // 设置工作线程数量
    pthreadPool->tidArr = (pthread_t *)calloc(pthreadPool->workerNum,sizeof(pthread_t)); // 为线程ID数组分配内存
    pthreadPool->mysql = mysql;
    initQueue(&pthreadPool->taskQueue); // 初始化任务队列
    return 0;
}

int main(int argc, char *argv[])
{
    // read config
    FILE* fp = fopen("server.conf", "r");
    char param1[256];
    char param2[256];
    char param3[256];
    
    u_cqueue_t *cqueue = init_queue();
    u_map_t *map = init_map();
    fscanf(fp, "%s %s %s", param1, param2, param3);
    fclose(fp);

//----------------------------------------------------------------
    //创建一个数据库锁
    mysqlMutex_t mysqlMute;
    //创建数据库
    MYSQL* mysql = mysql_init(NULL);
    // 一些初始化工作
    struct msg_t msg;
    MYSQL* ret = mysql_real_connect( mysql,
                                     "localhost", "root", "denghanwen159", "overflow_netdisk",
                                     0, NULL, 0);
    if (ret == NULL)
    {
        fprintf(stderr, "mysql_real_connect: %s\n", mysql_error(mysql));
        exit(1);
    }
//----------------------------------------------------------------
    threadPool_t threadPool; // 定义线程池结构体
    int workerNum = atoi(param3); // 将第三个命令行参数转换为整型，表示工作线程数量
    initThreadPool(&threadPool,workerNum, mysql); // 初始化线程池
    //netfd映射userCwd_t的哈希表

    int sockfd; // 定义套接字描述符
    tcpInit(param1,param2,&sockfd); // 初始化TCP连接，将服务器IP和端口号作为参数传入，并将返回的套接字描述符保存在sockfd中

    makeWorker(&threadPool); // 创建工作线程

    int epfd = epoll_create(1); // 创建epoll实例
    epollAdd(epfd,sockfd); // 将sockfd添加到epoll实例中进行监听
    struct epoll_event readyset[1024]; // 定义epoll事件数组，用于保存就绪的事件
    while(1)
    {
        int readynum = epoll_wait(epfd,readyset,1024,1000); // 等待事件发生，返回就绪的事件数量
        for(int i = 0; i < readynum; ++i)
        {
            if(readyset[i].data.fd == sockfd)
            {
                // 如果有新的客户端连接
                int netfd = accept(sockfd,NULL,NULL); // 接受连接请求，获取与客户端通信的套接字描述符
                printf("One client connected!\n");
                epollAdd(epfd, netfd);//将netfd放入监听队列
                add_user(netfd, cqueue, map);
            }
            else{
                add_user(readyset[i].data.fd, cqueue, map);
                int netfd = readyset[i].data.fd;
                //客户端来消息
                printf("netfd:%d send cmd!\n", netfd);
                user_t userInfo;
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
                int ret = analysis_cmd(&msg, netfd, mysql, &threadPool.taskQueue.netfd2Usr[netfd], &userInfo, &threadPool);
                if(ret > 0)
                {
                    //收文件
                    epollDel(epfd, netfd);
                }
                printf("username = %s\n", threadPool.taskQueue.netfd2Usr[netfd].user);
                printStack(&threadPool.taskQueue.netfd2Usr[netfd].dirstack);
            }
        }
        clear_user(epfd, map, cqueue);
    }
    free_map(map);
    free_queue(cqueue);
}

