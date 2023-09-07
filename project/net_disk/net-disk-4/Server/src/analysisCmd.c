#include "../Include/analysisCmd.h"
#include "../Include/jwt.h"

// 将接收到消息的第一部分转化为枚举
int cmd_analyse(const char *cmd);
// 反馈函数，正确返回正确信息，错误返回空包
void feedback(char* buf, struct msg_t* ret, int netfd, int flag);

int analysis_cmd(const struct msg_t* msg, int netfd, MYSQL* mysql, userCwd_t* userCwd, user_t* userInfo, threadPool_t* pthreadPool)
{
    printf("In analysis_cmd!\n");

    // 将接受到的消息转化为命令结构体
    cmd_t cmdinfo;
    bzero(&cmdinfo, sizeof(cmd_t));
    memcpy(&cmdinfo, msg->data, msg->size);

    char buf[4096] = {0}; // 用于存放回复客户端的消息
    char buf2[2048] = {0}; // 辅助缓冲区，用于拼接字符串等
    int flag = 0; // 标志位，用于表示函数成功与否

    // 初始化用于回复客户端的包
    struct msg_t ret;
    init_msg(&ret);

    printf("cmdinfo = %s\n", cmdinfo.cmd);

    switch (cmd_analyse(cmdinfo.cmd))
    {
    case COMMAND_LOGIN:
        memcpy(userInfo, cmdinfo.args, sizeof(cmdinfo.args));
        initCwd(userCwd, userInfo->name);
        flag = userLogin(userInfo->name, userInfo->password, mysql);
        if(flag == -1 || flag == 1) // -1表示函数错误，返回一个空包
        {
            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, 0);
            set_msgdata(&ret, buf);
            send_msg(netfd, &ret);
        }
        else if (flag == 0) // 0表示函数正确，返回正确信息
        {
            longMsg_t longMsg;
            bzero(&longMsg, sizeof(longMsg));
            getJwt(userInfo->name, longMsg.token);
            longMsg.netfd = netfd;

            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, sizeof(longMsg));
            set_msgdata(&ret, &longMsg);
 
            send_msg(netfd, &ret);

            initCwd(userCwd, userInfo->name);
        }
        break;

    case COMMAND_REGISTER:
        printf("args = %s\n", cmdinfo.args);
        memcpy(userInfo, cmdinfo.args, sizeof(cmdinfo.args));
        printf("userInfo = %s\n", userInfo->name);
        initCwd(userCwd, userInfo->name);
        flag = create_user(userInfo->name, userInfo->password, mysql);
        if(flag == -1 || flag == -2) // -1表示函数错误，返回一个空包
        {
            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, 0);
            set_msgdata(&ret, buf);
            send_msg(netfd, &ret);
        }
        else if (flag == 0) // 0表示函数正确，返回正确信息
        {
            longMsg_t longMsg;
            bzero(&longMsg, sizeof(longMsg));
            getJwt(userInfo->name, longMsg.token);

            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, sizeof(longMsg));
            set_msgdata(&ret, &longMsg);

            send_msg(netfd, &ret);
        }
        break;

    case COMMAND_LS:
        flag = myLs(userCwd, mysql, buf);
        printf("after ls: cwd = %d, username = %s\n", userCwd->cwd, userCwd->user);
        printf("ls = %s\n", buf);
        if(flag == -1) // -1表示函数错误，返回一个空包
        {
            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, 0);
            set_msgdata(&ret, buf);
            send_msg(netfd, &ret);
        }
        else if (flag == 0) // 0表示函数正确，返回正确信息
        {
            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, strlen(buf) + 1);
            set_msgdata(&ret, buf);
            send_msg(netfd, &ret);
        }
        break;

    case COMMAND_PWD:
        flag = myPwd(userCwd, buf);
        if(flag == -1) // -1表示函数错误，返回一个空包
        {
            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, 0);
            set_msgdata(&ret, buf);
            send_msg(netfd, &ret);
        }
        else if (flag == 0) // 0表示函数正确，返回正确信息
        {
            set_msgtype(&ret, R_INFO_T);
            set_msgsize(&ret, strlen(buf) + 1);
            set_msgdata(&ret, buf);
            send_msg(netfd, &ret);
        }
        break;

    case COMMAND_MKDIR:
        printf("before myMkdir: cwd = %d, username = %s\n", userCwd->cwd, userCwd->user);
        flag = myMkdir(userCwd, mysql, cmdinfo.args);
        printf("after myMkdir: cwd = %d, username = %s\n", userCwd->cwd, userCwd->user);
        feedback(buf, &ret, netfd, flag);
        break;

    case COMMAND_RMDIR: case COMMAND_RM:
        flag = myRmdir(userCwd, mysql, cmdinfo.args);
        feedback(buf, &ret, netfd, flag);
        break;

    case COMMAND_CD:
        flag = myCd(userCwd, mysql, cmdinfo.args);
        feedback(buf, &ret, netfd, flag);
        break;

    case COMMAND_GETFILE:
        //交给子线程

        pthread_mutex_lock(&pthreadPool->taskQueue.mutex); // 加锁，保护任务队列
        printf("enQueue!\n");
        enQueue(&pthreadPool->taskQueue, netfd, 1, cmdinfo.args); // 将套接字描述符加入任务队列
        printf("finish enQueue!\n");
        pthread_cond_signal(&pthreadPool->taskQueue.cond); // 发送信号，通知工作线程有任务可执行
        pthread_mutex_unlock(&pthreadPool->taskQueue.mutex); // 加锁，保护任务队列
        return 1;

    case COMMAND_PUTFILE:
        //交给子线程
        pthread_mutex_lock(&pthreadPool->taskQueue.mutex); // 加锁，保护任务队列
        printf("enQueue!\n");
        char buf[1] = {0};
        enQueue(&pthreadPool->taskQueue, netfd, 2, buf); // 将套接字描述符加入任务队列
        printf("finish enQueue!\n");
        pthread_cond_signal(&pthreadPool->taskQueue.cond); // 发送信号，通知工作线程有任务可执行
        pthread_mutex_unlock(&pthreadPool->taskQueue.mutex); // 加锁，保护任务队列
                                                             //
        return 2;

    case COMMAND_QUIT:
        close(netfd);
        printf("One client logout!\n");
        return -1;
    }
    return 0;
}

int cmd_analyse(const char *cmd){
    if(strcmp(cmd,"cd") == 0){
        return COMMAND_CD;
    }
    if(strcmp(cmd,"ls") == 0){
        return COMMAND_LS;
    }
    if(strcmp(cmd,"pwd") == 0){
        return COMMAND_PWD;
    }
    if(strcmp(cmd,"mkdir") == 0){
        return COMMAND_MKDIR;
    }
    if(strcmp(cmd,"rmdir") == 0){
        return COMMAND_RMDIR;
    }
    if(strcmp(cmd,"rm") == 0){
        return COMMAND_RM;
    }
    if(strcmp(cmd,"get") == 0){
        return COMMAND_GETFILE;
    }
    if(strcmp(cmd,"put") == 0){
        return COMMAND_PUTFILE;
    }
    if(strcmp(cmd,"quit") == 0){
        return COMMAND_QUIT;
    }
    if(strcmp(cmd,"login") == 0){
        return COMMAND_LOGIN;
    }
    if(strcmp(cmd,"register") == 0){
        return COMMAND_REGISTER;
    }
    return -1;
}

void feedback(char* buf, struct msg_t* ret, int netfd, int flag)
{
    if(flag == -1) // -1表示函数错误，返回一个空包
    {
        set_msgtype(ret, R_INFO_T);
        set_msgsize(ret, 0);
        set_msgdata(ret, buf);
        send_msg(netfd, ret);
    }
    else if (flag == 0) // 0表示函数正确，返回正确信息
    {
        strcpy(buf, "Sucess!");
        set_msgtype(ret, R_INFO_T);
        set_msgsize(ret, strlen(buf) + 1);
        set_msgdata(ret, buf);
        send_msg(netfd, ret);
    }
}

