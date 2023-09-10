#ifndef __SOCKET_IO_H__
#define __SOCKET_IO_H__

// 对套接字进行读写数据操作的类
class Socket_io
{
public:
    Socket_io(int fd);

    ~Socket_io();

    // 读数据
    int readn(char *buf, int len);

    // 读一行数据
    int read_line(char *buf, int len);

    // 写数据
    int writen(const char *buf, int len);

private:
    int _fd;
};
#endif