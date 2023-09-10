#include "Tcp_connection.h"
#include <iostream>

Tcp_connection::Tcp_connection(int fd)
    : _sock_io(fd),
      _sock(fd),
      _local_addr(get_local_addr()),
      _peer_addr(get_peer_addr())
{
}

Tcp_connection::~Tcp_connection()
{
}

// 发送数据
void Tcp_connection::send(const string &msg)
{
    _sock_io.writen(msg.c_str(), msg.size());
}

// 接收数据
string Tcp_connection::receive()
{
    char buf[65535] = {0};
    _sock_io.read_line(buf, sizeof(buf));

    return string(buf);
}

// 打印连接信息
void Tcp_connection::print_coninfo()
{
    std::cout << _local_addr.ip() << ":"
              << _local_addr.port() << "---->"
              << _peer_addr.ip() << ":"
              << _peer_addr.port() << std::endl;
}

// 获取本地地址
Inet_address Tcp_connection::get_local_addr()
{
    struct sockaddr_in local_addr;
    socklen_t len = sizeof(struct sockaddr);

    int ret = getsockname(_sock.fd(), (struct sockaddr *)&local_addr, &len);
    if (-1 == ret)
    {
        perror("get_local_addr fail");
    }

    return Inet_address(local_addr);
}

// 获取对端地址
Inet_address Tcp_connection::get_peer_addr()
{
    struct sockaddr_in peer_addr;
    socklen_t len = sizeof(struct sockaddr);

    int ret = getpeername(_sock.fd(), (struct sockaddr *)&peer_addr, &len);
    if (-1 == ret)
    {
        perror("get_peer_addr fail");
    }

    return Inet_address(peer_addr);
}

// 注册建立新连接回调函数
void Tcp_connection::set_new_connection_callback(const tcp_connection_callback &on_new_connection_callback)
{
    _on_new_connection = on_new_connection_callback;
}

// 注册消息到达回调函数
void Tcp_connection::set_message_callback(const tcp_connection_callback &on_message_callback)
{
    _on_message = on_message_callback;
}

// 注册连接断开回调函数
void Tcp_connection::set_close_callback(const tcp_connection_callback &on_close_callback)
{
    _on_close = on_close_callback;
}

// 执行建立新连接回调函数
void Tcp_connection::handle_new_connection_callback()
{
    if (_on_new_connection)
    {
        // shared_from_this() 用于获取当前 TcpConnection 对象的 shared_ptr，
        // 然后将其传递给 _on_new_connection 回调函数。
        // 这样做可以确保在回调函数执行期间，TcpConnection 对象的引用计数会增加，
        // 直到回调函数执行完毕后才会减少。
        // 这样可以确保在回调函数中安全地使用 TcpConnection 对象，
        // 而不会出现野指针或访问已释放内存的问题。
        _on_new_connection(shared_from_this());
    }
}

// 执行消息到达回调函数
void Tcp_connection::handle_message_callback()
{
    if (_on_message)
    {
        // 同上
        _on_message(shared_from_this());
    }
}

// 执行连接断开回调函数
void Tcp_connection::handle_close_callback()
{
    if (_on_close)
    {
        // 同上
        _on_close(shared_from_this());
    }
}

// 判断TCP连接是否已断开
bool Tcp_connection::is_close()
{
    char buf[10] = {0};
    // 使用了 MSG_PEEK 标志，
    // 即使接收缓冲区中有数据，这个函数执行后仍然可以继续使用 recv() 函数重新接收完整的数据
    int ret = ::recv(_sock.fd(), buf, sizeof(buf), MSG_PEEK);
    // ret = 0表示连接已关闭
    // 不等于0表示连接仍然打开
    return (0 == ret);
}