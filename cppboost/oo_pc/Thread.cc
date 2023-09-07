#include "Thread.h"
#include <stdio.h>

Thread::Thread()
    : _is_runing(false),
      _tid(0)
{
}

Thread::~Thread()
{
}
// 线程启动
void Thread::start()
{
    // 创建线程
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
    Thread *pth = static_cast<Thread *>(args);
    if (pth)
    {
        pth->run();
    }
    else
    {
        perror("pth = nullptr");
    }
    // 线程退出
    pthread_exit(nullptr);
}
