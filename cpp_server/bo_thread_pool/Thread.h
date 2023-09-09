#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <functional>

// Thread_callback被定义为一个函数类型
// 它不接受任何参数并返回void
// 它可以用于存储或传递符合这个函数签名的回调函数
using Thread_callback = std::function<void()>;

class Thread
{
public:
    Thread(Thread_callback &&cb);
    ~Thread();
    // 线程启动
    void start();
    // 线程停止
    void stop();

private:
    // 线程函数设置成静态成员函数的原因：
    // 创建线程的时候需要调用pthread_create(pthread_t *__restrict__ __newthread,
    //                                      const pthread_attr_t *__restrict__ __attr,
    //                                      void *(*__start_routine)(void *),
    //                                      void *__restrict__ __arg)
    // 此函数的第三个参数的形态是严格确定的不能进行更改，
    // 如果thread_func不是一个静态成员函数，那么thread_func就有一个隐含的this指针
    // 就与pthread_create函数的参数形态不一致
    static void *thread_func(void *args);
    bool _is_runing;            // 线程状态
    pthread_t _tid;             // 线程id
    Thread_callback _call_back; // 线程执行的任务
};
#endif