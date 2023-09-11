#ifndef __EVENT_LOOP_H__
#define __EVENT_LOOP_H__

#include "Acceptor.h"
#include "Tcp_connection.h"
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <sys/epoll.h>

using std::map;
using std::shared_ptr;
using std::vector;

using tcp_connection_ptr = std::shared_ptr<Tcp_connection>;
using tcp_connection_callback = std::function<void(const tcp_connection_ptr &)>;

class Event_loop
{

public:
    Event_loop(Acceptor &acceptor);

    ~Event_loop();

    // 启动事件循环，监听和处理事件
    void loop();

    // 停止事件循环
    void unloop();

    // 注册建立新连接回调函数
    void set_new_connection_callback(const tcp_connection_callback &&on_new_connection_callback);

    // 注册消息到达回调函数
    void set_message_callback(const tcp_connection_callback &&on_message_callback);

    // 注册连接断开回调函数
    void set_close_callback(const tcp_connection_callback &&on_close_callback);

private:
    // 等待 epoll 文件描述符上的事件
    void wait_epoll_fd();

    // 创建 epoll 文件描述符
    int create_epoll_fd();

    // 将文件描述符添加到 epoll 的读事件集合中
    void add_epoll_read_fd(int fd);

    // 将文件描述符从 epoll 的读事件集合中移除
    void del_epoll_read_fd(int fd);

    // 执行建立新连接回调函数
    void handle_new_connection_callback();

    // 执行消息到达回调函数
    void handle_message_callback(int fd);

    int _epfd;                                   // 存储epoll_create创建文件描述符
    bool _is_loop;                               // 标识事件循环是否正在运行
    Acceptor &_acceptor;                         // 用于监听新连接
    vector<struct epoll_event> _event_list;      // 存储 epoll 事件的列表
    map<int, shared_ptr<Tcp_connection>> _conns; // 保存已建立连接的信息，使用文件描述符作为键
    tcp_connection_callback _on_new_connection;  // 注册建立新连接回调函数
    tcp_connection_callback _on_message;         // 注册消息到达回调函数
    tcp_connection_callback _on_close;           // 注册连接断开回调函数
};
#endif