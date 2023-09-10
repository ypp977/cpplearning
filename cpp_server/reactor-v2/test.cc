#include "Tcp_connection.h"
#include "Acceptor.h"
#include "Event_loop.h"
#include <iostream>

using std::cout;
using std::endl;
using tcp_connection_ptr = std::shared_ptr<Tcp_connection>;

void on_new_connection(const tcp_connection_ptr &new_connection)
{
    cout << "new_connection:";
    new_connection->print_coninfo();
}

void on_message(const tcp_connection_ptr &message)
{
    // 接收客户端消息
    string msg = message->receive();
    cout << ">>recv client msg = " << msg << endl;
    // Todo
    //....

    message->send(msg);
}

void on_close(const tcp_connection_ptr &close)
{
    cout << "close:";
    close->print_coninfo();
}

void test()
{
    // 三次握手，放进全连接队列开始监听
    Acceptor acceptor("127.0.0.1", 9777);
    acceptor.ready();

    Event_loop loop(acceptor);
    loop.set_new_connection_callback(std::move(on_new_connection));
    loop.set_message_callback(std::move(on_message));
    loop.set_close_callback(std::move(on_close));

    // 开始循环
    loop.loop();
}
int main(int argc, char **argv)
{
    test();
    return 0;
}