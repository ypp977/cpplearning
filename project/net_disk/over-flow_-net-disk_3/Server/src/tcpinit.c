#include "../Include/head.h"

// TCP初始化函数，用于创建套接字并绑定地址
int tcpInit(char *ip, char *port,int *psockfd)
{
    *psockfd = socket(AF_INET,SOCK_STREAM,0); // 创建TCP套接字，返回的文件描述符保存在指针变量psockfd指向的内存中

    int arg = 1; // 设置SO_REUSEADDR选项的值为1，表示允许地址重用
    int ret = setsockopt(*psockfd,SOL_SOCKET,SO_REUSEADDR,&arg,sizeof(arg)); // 设置套接字选项，使得套接字可以立即重启而不需要等待一段时间
    ERROR_CHECK(ret,-1,"setsockopt"); // 检查操作是否成功

    struct sockaddr_in addr; // 定义IPv4地址结构体
    addr.sin_family = AF_INET; // 设置地址族为IPv4
    addr.sin_port = htons(atoi(port)); // 设置端口号，并将字符串类型的端口号转换为整型
    addr.sin_addr.s_addr = inet_addr(ip); // 设置IP地址，并将字符串类型的IP地址转换为网络字节序

    ret = bind(*psockfd,(struct sockaddr *)&addr,sizeof(addr)); // 绑定套接字和地址
    ERROR_CHECK(ret,-1,"bind"); // 检查操作是否成功

    listen(*psockfd,10); // 监听套接字，设置监听队列最大长度为10

    return 0;
}


