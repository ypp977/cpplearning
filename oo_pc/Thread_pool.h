#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "Task_queue.h"
#include <vector>
#include <memory>

using std::unique_ptr;
using std::vector;

class Thread;
class Thread_pool
{
    friend class Work_thread;

public:
    Thread_pool(size_t thread_num, size_t queue_capacity);
    ~Thread_pool();
    // 启动线程池
    void start();
    // 关闭线程池
    void stop();
    // 添加任务
    void add_task(Task *task);

private:
    // 获取任务
    Task *get_task();
    // 线程任务
    void thread_func();

    size_t _thread_num;                  // 线程数量
    size_t _queue_capacity;              // 任务队列最大容量
    vector<unique_ptr<Thread>> _threads; // 存放线程信息
    Task_queue _task_queue;              // 任务队列
    bool _is_exiting;                    // 线程池状态
};
#endif