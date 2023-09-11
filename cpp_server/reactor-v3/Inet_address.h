#ifndef __INET_ADDRESS_H__
#define __INET_ADDRESS_H__

#include <arpa/inet.h>
#include <string>

using std::string;

// 封装所有与网络地址相关的操作
class Inet_address
{
public:
    // 通过ip地址和端口号初始化_addr
    Inet_address(const string &ip, unsigned short port);
    // 通过struct sockaddr_in类型的参数初始化_addr
    Inet_address(const struct sockaddr_in &addr);
    ~Inet_address();
    // 获取_addr中点分十进制的ip地址
    string ip();
    // 获取_addr中主机字节序的端口号
    unsigned short port();
    // 获取Inet_address类中成员变量_addr的指针
    const struct sockaddr_in *get_innet_addr_ptr();

private:
    struct sockaddr_in _addr; // 存储网络地址信息
};
#endif