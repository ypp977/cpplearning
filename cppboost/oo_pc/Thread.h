#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>

class Thread
{
public:
    Thread();
    virtual ~Thread() = 0;
    // 线程启动
    void start();
    // 线程停止
    void stop();
    // 线程函数
    static void *thread_func(void *args);

private:
    // 运行
    virtual void run() = 0;
    bool _is_runing; // 线程状态
    pthread_t _tid;  // 线程id
};
#endif