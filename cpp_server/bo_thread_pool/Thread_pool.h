#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "Task_queue.h"
#include <vector>
#include <memory>
#include <functional>

using std::unique_ptr;
using std::vector;

using Task = std::function<void()>;

class Thread;
class Thread_pool
{
public:
    Thread_pool(size_t thread_num, size_t queue_capacity);

    ~Thread_pool();

    // 启动线程池
    void start();

    // 关闭线程池
    void stop();

    // 添加任务
    // 此处的形参使用移动语义
    // 这样做的好处是提高了效率，
    // 并且在某些情况下可以避免不必要的资源消耗
    void add_task(Task &&task);

private:
    // 获取任务
    Task get_task();

    // 做线程任务
    void thread_func();

    size_t _thread_num;                  // 线程数量
    size_t _queue_capacity;              // 任务队列最大容量
    vector<unique_ptr<Thread>> _threads; // 存放线程信息
    Task_queue _task_queue;              // 任务队列
    bool _is_exit;                       // 线程池状态
};
#endif