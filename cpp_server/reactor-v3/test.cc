#include "Tcp_connection.h"
#include "Tcp_server.h"
#include <iostream>

using std::cout;
using std::endl;

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
    Tcp_server server("127.0.0.1", 9777);
    server.set_all_callback(std::move(on_new_connection), std::move(on_message), std::move(on_close));

    // 开始
    server.start();
}
int main(int argc, char **argv)
{
    test();
    return 0;
}