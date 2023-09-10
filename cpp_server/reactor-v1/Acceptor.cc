#include "Acceptor.h"

// 接收ip地址和端口号作为参数初始化,
// 创建一个套接字对象bind端口号和ip地址
Acceptor::Acceptor(const string &ip, unsigned short port)
    : _addr(ip, port),
      _sock() // 三次握手成功之后要放进全连接队列中的fd
{
}

Acceptor::~Acceptor()
{
}

// 设置套接字属性
void Acceptor::ready()
{
    set_reuse_addr();
    set_reuse_port();
    // 将套接字绑定到指定的 IP 地址和端口号上。
    // 这是确保服务器能够监听特定 IP 和端口的连接请求的关键步骤
    bind();
    listen(); // 监听请求，建立三次握手之后放进全连接队列
}

// 接收客户端数据交互请求，从全连接队列中取出文件描述符
// 并返回与客户端建立的连接的文件描述符
int Acceptor::accept()
{
    int connfd = ::accept(_sock.fd(), nullptr, nullptr);
    if (-1 == connfd)
    {
        // 接受连接请求失败
        perror("accept fail");
        return -1;
    }
    // 返回与客户端建立的连接的文件描述符
    return connfd;
}

// 地址复用
void Acceptor::set_reuse_addr()
{
    int reuse = 1;
    int ret = setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    if (-1 == ret)
    {
        // 设置失败
        perror("set_reuse_addr fail");
        return;
    }
}

// 端口复用
void Acceptor::set_reuse_port()
{
    int reuse = 1;
    int ret = setsockopt(_sock.fd(), SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    if (-1 == ret)
    {
        // 设置失败
        perror("set_reuse_addr");
        return;
    }
}

// 将套接字绑定到ip地址和端口号上
void Acceptor::bind()
{
    // _addr.get_innet_addr_ptr()类型为const sockaddr_in *，
    // 但是bind的第二个参数类型为struct sockaddr *，需要强转
    int ret = ::bind(_sock.fd(), (struct sockaddr *)_addr.get_innet_addr_ptr(), sizeof(struct sockaddr));
    if (-1 == ret)
    {
        // 绑定失败
        perror("bind");
        return;
    }
}

// 监听连接请求
void Acceptor::listen()
{
    int ret = ::listen(_sock.fd(), 128);
    if (-1 == ret)
    {
        // 监听失败
        perror("listen");
        return;
    }
}