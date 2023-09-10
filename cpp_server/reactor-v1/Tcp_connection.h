#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include "Inet_address.h"
#include "Socket.h"
#include "Socket_io.h"

class Tcp_connection
{
public:
    explicit Tcp_connection(int fd);

    ~Tcp_connection();

    // 发送数据
    void send(const string &msg);

    // 接收数据
    string receive();

    void print_coninfo();

private:
    // 获取本地地址
    Inet_address get_local_addr();

    // 获取对端地址
    Inet_address get_peer_addr();

    Socket_io _sock_io;       // 进行套接字操作
    Socket _sock;             // 存储套接字
    Inet_address _local_addr; // 存储本地地址
    Inet_address _peer_addr;  // 存储对端地址
};
#endif