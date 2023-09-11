#include "Tcp_server.h"

Tcp_server::Tcp_server(const string &ip, unsigned short port)
    : _acceptor(ip, port),
      _loop(_acceptor)
{
}

Tcp_server::~Tcp_server()
{
}

// 服务器启动
void Tcp_server::start()
{
    // 开始监听
    _acceptor.ready();
    // epoll
    _loop.loop();
}

// 服务器停止
void Tcp_server::stop()
{
    _loop.unloop();
}

void Tcp_server::set_all_callback(tcp_connection_callback &&on_new_connection_callback, tcp_connection_callback &&on_message_callback, tcp_connection_callback &&on_close_callback)
{
    _loop.set_new_connection_callback(std::move(on_new_connection_callback));
    _loop.set_message_callback(std::move(on_message_callback));
    _loop.set_close_callback(std::move(on_close_callback));
}
