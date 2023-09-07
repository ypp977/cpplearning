#include "../Include/head.h"

// 将文件描述符添加到epoll实例中进行监听
int epollAdd(int epfd, int fd){
    struct epoll_event event; // 定义epoll事件结构体
    event.events = EPOLLIN; // 设置事件类型为可读
    event.data.fd = fd; // 设置事件关联的文件描述符
    epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event); // 将事件添加到epoll实例中进行监听
    return 0;
}

// 从epoll实例中删除文件描述符
int epollDel(int epfd, int fd){
    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,NULL); // 从epoll实例中删除指定文件描述符的事件
    return 0;
}

// 这两个函数分别用于将文件描述符添加到epoll实例中进行监听和从epoll实例中删除文件描述符。

