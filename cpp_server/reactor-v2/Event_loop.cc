#include "Event_loop.h"
#include <unistd.h>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

Event_loop::Event_loop(Acceptor &acceptor)
    : _epfd(create_epoll_fd()),
      _is_loop(false),
      _acceptor(acceptor),
      _event_list(1024)
{
    // 将文件描述符listenfd放在红黑树上进行监听
    // 该listenfd就是Socket类创建的数据成员
    add_epoll_read_fd(_acceptor.fd());
}

Event_loop::~Event_loop()
{
    close(_epfd);
}

// 循环执行wait_epoll_fd
void Event_loop::loop()
{
    _is_loop = true;
    while (_is_loop)
    {
        wait_epoll_fd();
    }
}

// 停止事件循环
void Event_loop::unloop()
{
    _is_loop = false;
}

// 注册建立新连接回调函数
void Event_loop::set_new_connection_callback(const tcp_connection_callback &&on_new_connection_callback)
{
    _on_new_connection = std::move(on_new_connection_callback);
}

// 注册消息到达回调函数
void Event_loop::set_message_callback(const tcp_connection_callback &&on_message_callback)
{
    _on_message = std::move(on_message_callback);
}

// 注册连接断开回调函数
void Event_loop::set_close_callback(const tcp_connection_callback &&on_close_callback)
{
    _on_close = std::move(on_close_callback);
}

// 等待 epoll 文件描述符上的事件
void Event_loop::wait_epoll_fd()
{
    int ready_num = 0; // 就绪数量
    do
    {
        // 轮询
        // 等待文件描述符的就绪事件。将就绪的事件存储在 _event_list 中
        ready_num = ::epoll_wait(_epfd, &*_event_list.begin(), _event_list.size(), 3000);
    } while (-1 == ready_num && errno == EINTR); // 发生中断，继续进行轮询

    if (-1 == ready_num)
    {
        // 发生错误
        cerr << "-1 == ready_num" << endl;
    }
    else if (0 == ready_num)
    {
        // 超时
        cout << ">>epoll_wait tiomeout" << endl;
    }
    else
    {
        // 监听的文件描述符是1024
        if (ready_num == (int)_event_list.size())
        {
            // 扩容
            _event_list.resize(2 * ready_num);
        }
        // 遍历就绪队列
        for (int i = 0; i < ready_num; ++i)
        {
            int fd = _event_list[i].data.fd;
            // 判断是否是listen_fd
            if (_acceptor.fd() == fd)
            {
                // 有新的连接请求
                if (_event_list[i].events & EPOLLIN)
                {
                    // 建立新连接
                    handle_new_connection_callback();
                }
            }
            else
            {
                // 检查就绪事件是否为可读事件 EPOLLIN，
                // 如果是，则调用 handle_message_callback(fd) 处理数据交互
                if (_event_list[i].events & EPOLLIN)
                {
                    // 数据交互
                    handle_message_callback(fd);
                }
            }
        }
    }
}

// 创建新的文件描述符
int Event_loop::create_epoll_fd()
{
    // 创建 epoll 文件描述符
    int fd = ::epoll_create1(0);
    if (0 > fd)
    {
        // 创建失败
        perror("create_epoll_fd");
        return fd;
    }
    // 成功返回文件描述符
    return fd;
}

// 将文件描述符添加到 epoll 的读事件集合中
void Event_loop::add_epoll_read_fd(int fd)
{
    // 存储要监听的事件
    struct epoll_event event;
    // 监听可读事件
    event.events = EPOLLIN;
    // 设置监听的文件描述符 fd
    event.data.fd = fd;
    // 将文件描述符及事件信息添加到监听列表中
    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);
    if (ret < 0)
    {
        // 添加失败
        perror("add_epoll_read_fd");
        return;
    }
}

// 将文件描述符从 epoll 的读事件集合中移除
void Event_loop::del_epoll_read_fd(int fd)
{
    // 配置要移除的事件
    struct epoll_event event;
    // 移除的事件为可读事件
    event.events = EPOLLIN;
    // 移除的文件描述符 fd
    event.data.fd = fd;
    // 将文件描述符及事件信息从监听列表中移除
    int ret = ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &event);
    if (ret < 0)
    {
        // 移除失败
        perror("del_epoll_read_fd");
        return;
    }
}

// 执行建立新连接回调函数
void Event_loop::handle_new_connection_callback()
{
    // 接受新的连接请求，并将返回的套接字文件描述符
    int connection_fd = _acceptor.accept();
    if (connection_fd < 0)
    {
        // 接受连接失败
        perror("handle_new_connection_callback");
        return;
    }
    // 将新连接的套接字文件描述符添加到 epoll 的读事件监听列表中
    add_epoll_read_fd(connection_fd);

    // 设置新连接的回调函数
    tcp_connection_ptr connect_ptr(new Tcp_connection(connection_fd));

    connect_ptr->set_new_connection_callback(_on_new_connection);
    connect_ptr->set_message_callback(_on_message);
    connect_ptr->set_close_callback(_on_close);
    // 新连接的套接字文件描述符和 connect_ptr 对象存储到 _conns 容器中
    _conns.insert(std::make_pair(connection_fd, connect_ptr));
    // 用于处理新连接的回调操作
    connect_ptr->handle_new_connection_callback();
}

// 执行消息到达回调函数
void Event_loop::handle_message_callback(int fd)
{
    auto it = _conns.find(fd);
    if (it != _conns.end())
    {
        // 判断连接是否已经关闭
        bool flag = it->second->is_close();
        if (flag)
        {
            // 关闭
            // 处理连接关闭的回调操作
            it->second->handle_close_callback();
            // 将文件描述符从 epoll 实例中移除
            del_epoll_read_fd(fd);
            // _conns 容器中移除该连接对象
            _conns.erase(it);
        }
        else
        {
            // 未关闭
            // 处理消息到达的回调操作
            it->second->handle_message_callback();
        }
    }
    else
    {
        cout << "该连接不存在" << endl;
        return;
    }
}