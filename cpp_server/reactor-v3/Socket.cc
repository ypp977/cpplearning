#include "Socket.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

// 无参创建套接字文件描述符
Socket::Socket()
{
    // 第一个参数
    // AF_INET代表使用的网络协议为IPv4
    // AF_INET6代表使用的网络协议为IPv4

    // 第二个参数
    // SOCK_STREAM：流式套接字，
    // 提供面向连接的、可靠的、双向的字节流传输，使用 TCP 协议。
    // SOCK_DGRAM：数据报套接字，
    // 提供无连接的、不可靠的、固定最大长度的数据传输，使用 UDP 协议

    // 第三个参数
    // 一般填0，表示自动选择套接字使用的协议

    // 使用socket创建套接字
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);

    // 检查套接字是否创建成功
    if (_fd < 0)
    {
        perror("Socket fail");
    }
}

// 带参创建套接字文件描述符
Socket::Socket(int fd)
    : _fd(fd)
{
}

// 关闭套接字文件描述符
Socket::~Socket()
{
    close(_fd);
}

// 返回套接字文件描述符
int Socket::fd() const
{
    return _fd;
}

// 关闭套接字的写入端
void Socket::shut_down_write()
{
    int ret = shutdown(_fd, SHUT_WR);
    if (ret)
    {
        perror("shut_down_write fail");
        return;
    }
}