#include "Thread_pool.h"
#include "Thread.h"
#include <unistd.h>

Thread_pool::Thread_pool(size_t thread_num, size_t queue_capacity)
    : _thread_num(thread_num),
      _queue_capacity(queue_capacity),
      _task_queue(_queue_capacity),
      _is_exit(true)
{
    _threads.reserve(_thread_num);
}

Thread_pool::~Thread_pool()
{
}

void Thread_pool::start()
{
    // 遍历新建Work_thread并把线程vector
    for (size_t idx = 0; idx < _thread_num; ++idx)
    {
        // 线程池创建线程并给线程分配任务
        // 通过使用std::bind函数将Thread_pool::thread_func与当前的Thread_pool对象绑定起来，
        // 并将绑定后的结果作为回调函数传递给了Thread类的构造函数。
        // 这样做的目的是在Thread类内部调用Thread_pool对象的成员函数作为回调函数，
        // 实现了回调的功能
        unique_ptr<Thread> th(new Thread(std::bind(&Thread_pool::thread_func, this)));
        // 放进vector中
        _threads.push_back(std::move(th));
    }
    // 遍历启动线程池所有线程
    for (auto &elem : _threads)
    {
        elem->start();
    }
    // 修改线程池状态
    _is_exit = false;
}

void Thread_pool::stop()
{
    // 判断任务队列是否为空
    while (!_task_queue.empty())
    {
        sleep(1);
    }
    // 修改线程池状态
    // 必须先修改状态，否则被唤醒的线程会被分配新任务
    // 导致线程无法关闭
    _is_exit = true;
    // 把所有沉睡的线程全部唤醒
    _task_queue.wake_up();
    // 遍历关闭线程池
    for (auto &th : _threads)
    {
        th->stop();
    }
}

// 此处的形参使用移动语义
// 这样做的好处是提高了效率，
// 并且在某些情况下可以避免不必要的资源消耗
void Thread_pool::add_task(Task &&task)
{
    if (task)
    {
        _task_queue.push(std::move(task));
    }
}

Task Thread_pool::get_task()
{
    return _task_queue.pop();
}

// 从任务队列取任务
// 运行任务
void Thread_pool::thread_func()
{
    // 判断线程池是否在运行状态
    // 处于运行状态子线程就会一直做任务
    while (!_is_exit)
    {
        // 取任务
        Task task_call_back = get_task();
        if (task_call_back)
        {
            // 任务不为空则运行
            task_call_back();
        }
    }
}
