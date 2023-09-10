#include "Socket_io.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>

Socket_io::Socket_io(int fd)
    : _fd(fd)
{
}

Socket_io::~Socket_io()
{
    close(_fd);
}

// 读数据
// 代码的目的就是：如果想读len个字节，就循环
// 调用read函数，如果没有read失败，那么就
// 可以保证读到len个字节后退出
int Socket_io::readn(char *buf, int len)
{
    int ret = 0, left = len; // ret存储每次读的字节数，left存储剩余未读字节数
    char *pbuf = buf;

    while (left > 0)
    {
        ret = read(_fd, pbuf, left);
        if (-1 == ret && errno == EINTR)
        {
            // 发生中断，重新读
            continue;
        }
        else if (-1 == ret)
        {
            // 发生错误，返回已读字节数
            perror("readn error -1");
            return len - left;
        }
        else if (0 == ret)
        {
            // 读取完毕跳出循环
            break;
        }
        else
        {
            pbuf += ret; // 更新pbuf指针
            left -= ret; // 更新未读字节数
        }
    }
    // 返回总字节数和已读字节数之差
    return len - left;
}

// 读一行数据
int Socket_io::read_line(char *buf, int len)
{
    // ret存储每次读的字节数,total存储已读总字节数
    int ret = 0, total = 0;
    // left存储剩余未读字节数
    // len - 1为了在读取文本行时留出一个位置用于存储字符串结尾的空字符 '\0'
    int left = len - 1;
    char *pbuf = buf;

    while (left > 0)
    {
        // MSG_PEEK不会将缓冲区中的数据进行清空,只会进行拷贝操作
        ret = recv(_fd, pbuf, left, MSG_PEEK);
        if (-1 == ret && errno == EINTR)
        {
            // 发生中断，重新读
            continue;
        }
        else if (-1 == ret)
        {
            // 发生错误，返回已读字节数
            perror("read_line error -1");
            return len - left;
        }
        else if (0 == ret)
        {
            // 读取完毕跳出循环
            break;
        }
        else
        {
            // 遍历缓冲区
            for (int idx = 0; idx < ret; ++idx)
            {
                // 判断是否找到完整的一行
                if ('\n' == pbuf[idx])
                {
                    int line_len = idx + 1; // 将line_len设置成文本行的长度
                    readn(pbuf, line_len);  // 读取line_len到缓冲区中
                    pbuf += line_len;       // 更新pbuf指针
                    *pbuf = '\0';           // 添加空字符'\0'

                    return total + line_len; // 返回一行总字节数
                }
            }
            readn(pbuf, ret); // 读取ret字节的数据到pbuf所指向的缓冲区
            total += ret;     // 累加已读总字节数
            pbuf += ret;      // 更新pbuf指针
            left -= ret;      // 更新未读字节数
        }
    }
    // 返回总字节数和已读字节数之差
    return len - left;
}

// 写数据
int Socket_io::writen(const char *buf, int len)
{
    int ret = 0, left = len; // ret存储每次写的字节数，left存储剩余未写字节数
    const char *pbuf = buf;

    while (left > 0)
    {
        ret = write(_fd, pbuf, left);
        if (-1 == ret && errno == EINTR)
        {
            // 发生中断，重新写
            continue;
        }
        else if (-1 == ret)
        {
            // 发生错误，返回已写字节数
            perror("writen error -1");
            return len - left;
        }
        else if (0 == ret)
        {
            // 写完毕跳出循环
            break;
        }
        else
        {
            pbuf += ret; // pbuf指针后移ret位
            left -= ret; // 更新未读字节数
        }
    }
    // 返回总字节数和已写字节数之差
    return len - left;
}