#include "Inet_address.h"
#include <string.h>

// 通过ip地址和端口号初始化_addr
Inet_address::Inet_address(const string &ip, unsigned short port)
{
    //  htons() 函数用于将16位的主机字节序转换为网络字节序。
    //  htonl() 函数用于将32位的主机字节序转换为网络字节序。
    //  inet_addr() 函数用于将一个IPv4地址的字符串表示转换为网络字节序的32位无符号整数

    // 在 struct sockaddr_in 中，
    // sin_port 字段是一个16位的端口号，因此需要使用 htons() 函数进行字节序转换

    // 重置_addr
    ::bzero(&_addr, sizeof(struct sockaddr_in));
    // 初始化_addr
    _addr.sin_family = AF_INET;                    // 指定地址族，IPV4设置为AF_INET
    _addr.sin_port = htons(port);                  // 设置端口
    _addr.sin_addr.s_addr = inet_addr(ip.c_str()); // 设置ip地址
}

// 通过struct sockaddr_in类型的参数初始化_addr
Inet_address::Inet_address(const struct sockaddr_in &addr)
    : _addr(addr)
{
}

Inet_address::~Inet_address()
{
}

// 获取_addr中点分十进制的ip地址
string Inet_address::ip()
{
    // 通过调用 inet_ntoa(_addr.sin_addr)，
    // 将 sin_addr 字段传递给 inet_ntoa() 函数，
    // 它会将网络字节序的IPv4地址转换为点分十进制的字符串表示。
    // 这样可以方便地将网络字节序的IP地址转换为人类可读的形式，
    return string(inet_ntoa(_addr.sin_addr));
}

// 获取_addr中主机字节序的端口号
unsigned short Inet_address::port()
{
    // 将_addr结构体中存储的网络字节序形式的端口号转换为主机字节序
    return ntohs(_addr.sin_port);
}

// 获取Inet_address类中成员变量_addr的指针
const struct sockaddr_in *Inet_address::get_innet_addr_ptr()
{
    return &_addr;
}