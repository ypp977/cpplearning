#include "Thread_pool.h"
#include "Work_thread.h"

Thread_pool::Thread_pool(size_t thread_num, size_t queue_capacity)
    : _thread_num(thread_num),
      _queue_capacity(queue_capacity),
      _task_queue(queue_capacity),
      _is_exiting(true)
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
        // 新建Work_thread
        unique_ptr<Thread> th(new Work_thread(*this));
        // 放进vector中
        _threads.push_back(std::move(th));
    }
    // 遍历启动线程池所有线程
    for (auto &elem : _threads)
    {
        elem->start();
    }
    // 修改线程池状态
    _is_exiting = false;
}

void Thread_pool::stop()
{
}

void Thread_pool::add_task(Task *task)
{
    _task_queue.push(task);
}

Task *Thread_pool::get_task()
{
    return _task_queue.pop();
}

void Thread_pool::thread_func()
{
}
