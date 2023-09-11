#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "Acceptor.h"
#include "Event_loop.h"

class Tcp_server
{
public:
    Tcp_server(const string &ip, unsigned short port);

    ~Tcp_server();

    void start();

    void stop();

    void set_all_callback(tcp_connection_callback &&on_new_connection_callback, tcp_connection_callback &&on_message_callback, tcp_connection_callback &&on_close_callback);

private:
    Acceptor _acceptor;
    Event_loop _loop;
};
#endif