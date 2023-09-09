#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__

#include "Condition.h"
#include "Mutex_lock.h"
#include <queue>
#include <functional>

using std::queue;

using Elem_type = std::function<void()>;

class Task_queue
{
public:
    Task_queue(size_t capacity);

    ~Task_queue();

    // 入队
    void push(const Elem_type &&value);

    // 出队
    Elem_type pop();

    // 判空
    bool empty() const;

    // 判满
    bool full() const;

    // 唤醒沉睡进程
    void wake_up();

private:
    size_t _queue_capacity;     // 队列最大容量
    queue<Elem_type> _queue;    // 任务队列
    Mutex_lock _mutex;          // 锁
    Condition _notify_consumer; // 通知消费者
    Condition _notify_producer; // 通知生产者
    bool _can_sleep;            // 线程能否再次睡眠
};
#endif