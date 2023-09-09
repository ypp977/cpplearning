#include "Thread.h"
#include <stdio.h>

// 此处的形参使用移动语义
// 这样做的好处是提高了效率，
// 并且在某些情况下可以避免不必要的资源消耗
//_call_back(std::move(cb))同上
Thread::Thread(Thread_callback &&cb)
    : _is_runing(false),
      _tid(0),
      _call_back(std::move(cb)) // 注册回调函数
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
    // 第四个参数不传this指针的话，在运行thread_func函数时会报段错误
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
    // 将args强转成Thread *，且args必须是个非空值
    // 因为静态成员函数没有this指针
    // 但在该静态成员函数中调用该类的成员变量
    // 需要一个此类类型的指针才可以调用
    Thread *pth = static_cast<Thread *>(args);
    if (pth)
    {
        // 执行回调函数
        pth->_call_back();
    }
    else
    {
        perror("pth = nullptr");
    }
    // 线程退出
    pthread_exit(nullptr);
}
