#ifndef __WORK_THREAD_H__
#define __WORK_THREAD_H__

#include "Thread.h"
#include "Thread_pool.h"

class Work_thread
    : public Thread
{
public:
    Work_thread(Thread_pool &thread_pool)
        : _thread_pool(thread_pool)
    {
    }
    ~Work_thread()
    {
    }

    void run() override
    {
        // 完成线程池交给工作线程的任务
        // 也就是取任务，运行任务
        // 即线程池的do_task() 所运行的流程
        _thread_pool.do_task();
    }

private:
    Thread_pool &_thread_pool;
};
#endif