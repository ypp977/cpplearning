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
    // 线程函数设置成静态成员函数的原因：
    // 创建线程的时候需要调用pthread_create(pthread_t *__restrict__ __newthread,
    //                                      const pthread_attr_t *__restrict__ __attr,
    //                                      void *(*__start_routine)(void *),
    //                                      void *__restrict__ __arg)
    // 第三个参数的形态是严格确定的不能进行更改，
    // 如果thread_func不是一个静态成员函数，那么thread_func就有一个隐含的this指针
    // 就与pthread_create函数的参数形态不一致
    // 所以需要设置成静态成员函数消除隐含的this指针，与其函数形态保持一致
    static void *thread_func(void *args);

private:
    // 运行
    virtual void run() = 0;
    bool _is_runing; // 线程状态
    pthread_t _tid;  // 线程id
};
#endif