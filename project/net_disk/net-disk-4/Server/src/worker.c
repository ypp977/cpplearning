#include "../Include/analysisCmd.h"
#include "../Include/jwt.h"
#include "../Include/head.h"
#include "../Include/dir.h"

// 线程函数，用于工作线程处理任务
void *threadFunc(void *arg){
    threadPool_t * pthreadPool = (threadPool_t *)arg; // 将传入的参数转换为线程池结构体指针

        printf("work!\n");
    while(1){
        // 接收任务
        pthread_mutex_lock(&pthreadPool->taskQueue.mutex); // 加锁，保护任务队列
        while(pthreadPool->taskQueue.queueSize <= 0){ // 如果任务队列为空，则等待任务到来
            pthread_cond_wait(&pthreadPool->taskQueue.cond,&pthreadPool->taskQueue.mutex);
        }
        int netfd = pthreadPool->taskQueue.pFront->netfd; // 取出任务队列头部的套接字描述符
        int flag = pthreadPool->taskQueue.pFront->flag;//分辨接收文件命令的flag
        char args[4096] = {0};//参数
        strncpy(args, pthreadPool->taskQueue.pFront->buf, BUFSIZE);
        deQueue(&pthreadPool->taskQueue); // 从任务队列中删除该任务
        pthread_mutex_unlock(&pthreadPool->taskQueue.mutex); // 解锁
        printf("netfd = %d\n", netfd);
//--------------------------------------------------------------------------------------
        userCwd_t userCwd;
//--------------------------------------------------------------------------------------

        // 接收username和token
        struct msg_t msg;
        //发送回馈
        printf("child connect!\n");
        char buf[4096] = {0};
        strcpy(buf, "1");
        set_msgtype(&msg, R_INFO_T);
        set_msgsize(&msg, strlen(buf)+1);
        set_msgdata(&msg, buf);
        send_msg(netfd, &msg);
        printf("child connect!\n");
        init_msg(&msg);
        if (recv_msg(netfd, &msg) == -1)
        {
            printf("One client logout or error!\n");
            close(netfd);
            break;
        }
        longMsg_t longMsg;
        memcpy(&longMsg, msg.data, msg.size);
        char token[BUFSIZE];
        printf("longMsg.username = %s\n", longMsg.username);
        printf("longMsg.token = %s\n", longMsg.token);
        getJwt(longMsg.username, token);
        printf("token = %s\n", token);
        if(strcmp(token, longMsg.token) != 0 )
        {
            //token识别不通过;
            init_msg(&msg);
            bzero(buf,sizeof(buf));
            strcpy(buf, "-1");
            set_msgtype(&msg, R_INFO_T);
            set_msgsize(&msg, strlen(buf) + 1);
            set_msgdata(&msg, buf);
            send_msg(netfd, &msg);
            close(netfd);

        }
        else{
            init_msg(&msg);
            bzero(buf,sizeof(buf));
            strcpy(buf, "1");
            set_msgtype(&msg, R_INFO_T);
            set_msgsize(&msg, strlen(buf) + 1);
            set_msgdata(&msg, buf);
            send_msg(netfd, &msg);
            char path[4096] = {0};
            //--------------------------------------------------------------------------------------
            userCwd_t userCwd;

            memcpy(&userCwd, &pthreadPool->taskQueue.netfd2Usr[longMsg.netfd], sizeof(userCwd_t));
            //--------------------------------------------------------------------------------------
            if(flag == 1)
            { 
                //接收文件
                getPath(&userCwd.dirstack, path);
                if(strcmp(path, "/"))
                    strcat(path, "/");
                strcat(path, args);
                int ret = get_file(netfd, pthreadPool->mysql, longMsg.username, path);
                //        if(!ret)
                //       ADDLOG("get_file sucess!", 1, pthreadPool->mysql);
                //   else
                //       ADDLOG("get_file failed!", 0, mysql);
            }
            else if(flag == 2)
            {
                //传输文件
                getPath(&userCwd.dirstack, path);
                int ret = put_file(netfd, pthreadPool->mysql, longMsg.username, path);
            }

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


