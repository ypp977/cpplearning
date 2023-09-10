#include "Tcp_connection.h"
#include <iostream>

Tcp_connection::Tcp_connection(int fd)
    : _sock_io(fd),
      _sock(fd),
      _local_addr(get_local_addr()),
      _peer_addr(get_peer_addr())
{
}

Tcp_connection::~Tcp_connection()
{
}

// 发送数据
void Tcp_connection::send(const string &msg)
{
    _sock_io.writen(msg.c_str(), msg.size());
}

// 接收数据
string Tcp_connection::receive()
{
    char buf[65535] = {0};
    _sock_io.read_line(buf, sizeof(buf));

    return string(buf);
}

void Tcp_connection::print_coninfo()
{
    std::cout << _local_addr.ip() << ":"
              << _local_addr.port() << "---->"
              << _peer_addr.ip() << ":"
              << _peer_addr.port() << std::endl;
}

// 获取本地地址
Inet_address Tcp_connection::get_local_addr()
{
    struct sockaddr_in local_addr;
    socklen_t len = sizeof(struct sockaddr);

    int ret = getsockname(_sock.fd(), (struct sockaddr *)&local_addr, &len);
    if (-1 == ret)
    {
        perror("get_local_addr fail");
    }

    return Inet_address(local_addr);
}

// 获取对端地址
Inet_address Tcp_connection::get_peer_addr()
{
    struct sockaddr_in peer_addr;
    socklen_t len = sizeof(struct sockaddr);

    int ret = getpeername(_sock.fd(), (struct sockaddr *)&peer_addr, &len);
    if (-1 == ret)
    {
        perror("get_peer_addr fail");
    }

    return Inet_address(peer_addr);
}