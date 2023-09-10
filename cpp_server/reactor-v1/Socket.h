#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "No_copy.h"

// 封装所有与文件描述符相关的操作
class Socket
    : No_copy
{
public:
    // 无参创建套接字文件描述符
    Socket();

    // 带参创建套接字文件描述符
    // explicit只能被显式调用，禁止隐式类型转换
    explicit Socket(int fd);

    // 关闭套接字文件描述符
    ~Socket();

    // 返回套接字文件描述符
    int fd() const;

    // 关闭套接字的写入端
    void shut_down_write();

private:
    int _fd; // 文件描述符
};
#endif