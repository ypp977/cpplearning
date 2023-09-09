#include "Thread.h"
#include <stdio.h>

Thread::Thread()
    : _is_runing(false),
      _tid(0)
{
}

Thread::~Thread()
{
    if (_is_runing)
    {
        int ret = pthread_detach(_tid);
        if (ret)
        {
            perror("pthread_detach");
        }
    }
}

// 线程启动
void Thread::start()
{
    // 创建线程
    // 第四个传递this指针是为了在静态成员函数thread_func中体现多态
    int ret = pthread_create(&_tid, nullptr, thread_func, this);
    if (ret)
    {
        perror("thread start fail");
        return;
    }
    // 修改线程运行状态
    _is_runing = true;
}

// 线程停止
void Thread::stop()
{
    // 判断线程是否正在运行
    if (_is_runing)
    {
        // 等待子线程关闭
        int ret = pthread_join(_tid, nullptr);
        if (ret)
        {
            perror("thread stop fail");
            return;
        }
        // 修改线程运行状态
        _is_runing = false;
    }
}

// 线程函数
void *Thread::thread_func(void *args)
{
    // 将args强转成Thread *
    // 因为静态成员函数没有this指针，且args不能为空
    // 否则就不无法调用run方法，体现多态
    // 也就无法调用Work_thread中的run方法
    // 所以需要pthread_create第四个参数传递进来
    Thread *pth = static_cast<Thread *>(args);
    if (pth)
    {
        // 多态
        // Work_thread中的run方法
        pth->run();
    }
    else
    {
        perror("pth = nullptr");
    }
    // 线程退出
    pthread_exit(nullptr);
}
