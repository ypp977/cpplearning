#include "Tcp_connection.h"
#include "Acceptor.h"
#include <iostream>

void test()
{
    // 三次握手，放进全连接队列开始监听
    Acceptor acceptor("127.0.0.1", 9777);
    acceptor.ready();

    // 从全连接队列中取出套接字，
    // 建立TCP连接生成新的可以进行数据交互的套接字
    Tcp_connection tcp_con(acceptor.accept());

    // 打印连接信息
    tcp_con.print_coninfo();

    while (1)
    {
        std::cout << ">>recv msg from client: " << tcp_con.receive() << std::endl;
        tcp_con.send("hello 977\n");
    }
}
int main(int argc, char **argv)
{
    test();
    return 0;
}