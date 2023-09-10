#ifndef __TCP_CONNECTION_H__
#define __TCP_CONNECTION_H__

#include "Inet_address.h"
#include "Socket.h"
#include "Socket_io.h"
#include <memory>
#include <functional>

class Tcp_connection
    : public std::enable_shared_from_this<Tcp_connection>
{
    using tcp_connection_ptr = std::shared_ptr<Tcp_connection>;
    using tcp_connection_callback = std::function<void(const tcp_connection_ptr &)>;

public:
    explicit Tcp_connection(int fd);

    ~Tcp_connection();

    // 发送数据
    void send(const string &msg);

    // 接收数据
    string receive();

    // 打印连接信息
    void print_coninfo();

    // 注册建立新连接回调函数
    // 尽管移动语义可以提高性能，
    // 但在回调函数的情况下，通常使用传值的方式传递回调函数，以确保回调函数的正确性和可用性。
    // 这是一种常见的做法，以保证回调函数可以在需要时正确地被调用
    // 因为回调函数通常需要在整个程序的执行过程中持续存在，并且在特定的事件触发时被调用。
    // 如果我们使用移动语义将回调函数传递给接口，
    // 那么原始的调用方将失去对回调函数的所有权，导致在回调函数需要被触发时无法使用它
    void set_new_connection_callback(const tcp_connection_callback &on_new_connection_callback);

    // 注册消息到达回调函数
    void set_message_callback(const tcp_connection_callback &on_message_callback);

    // 注册连接断开回调函数
    void set_close_callback(const tcp_connection_callback &on_close_callback);

    // 执行建立新连接回调函数
    void handle_new_connection_callback();

    // 执行消息到达回调函数
    void handle_message_callback();

    // 执行连接断开回调函数
    void handle_close_callback();

    // 判断TCP连接是否已断开
    bool is_close();

private:
    // 获取本地地址
    Inet_address get_local_addr();

    // 获取对端地址
    Inet_address get_peer_addr();

    Socket_io _sock_io;                         // 进行套接字操作
    Socket _sock;                               // 存储套接字
    Inet_address _local_addr;                   // 存储本地地址
    Inet_address _peer_addr;                    // 存储对端地址
    tcp_connection_callback _on_new_connection; // 注册建立新连接回调函数
    tcp_connection_callback _on_message;        // 注册消息到达回调函数
    tcp_connection_callback _on_close;          // 注册连接断开回调函数
};
#endif