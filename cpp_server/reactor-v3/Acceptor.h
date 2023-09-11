#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Inet_address.h"
#include "Socket.h"
#include <string>

using std::string;

class Acceptor
{
public:
    // 接收ip地址和端口号作为参数初始化
    Acceptor(const string &ip, unsigned short port);

    ~Acceptor();

    // 设置套接字属性
    void ready();

    // 地址复用
    void set_reuse_addr();

    // 端口复用
    void set_reuse_port();

    // 将套接字绑定到ip地址和端口号上
    void bind();

    // 监听连接请求
    void listen();

    // 接收客户端连接请求，
    // 并返回与客户端建立的连接的文件描述符
    int accept();

    // 返回套接字文件描述符
    int fd() const;

private:
    Inet_address _addr; // 存储ip地址和端口
    Socket _sock;       // 处理套接字操作
};
#endif