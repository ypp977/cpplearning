#include "Task_queue.h"
#include "Mutex_lock.h"

Task_queue::Task_queue(size_t capacity)
    : _queue_capacity(capacity),
      _queue(),
      _mutex(),
      _notify_consumer(_mutex),
      _notify_producer(_mutex),
      _can_sleep(true)
{
}

Task_queue::~Task_queue()
{
}

// 入队
// 此处的形参使用移动语义
// 这样做的好处是提高了效率，
// 并且在某些情况下可以避免不必要的资源消耗
void Task_queue::push(const Elem_type &&value)
{
    // 利用RAII技术实现自动上锁解锁
    {
        Auto_lock al(_mutex);
        // 循环判满
        while (full())
        {
            // 满通知生产者等待
            _notify_producer.wait();
        }
        // 不满则push
        _queue.push(value);
        // 通知消费者消费
        _notify_consumer.notify();
    }
}

// 出队
Elem_type Task_queue::pop()
{
    // 利用RAII技术实现自动上锁解锁
    Auto_lock al(_mutex);
    // 循环判空
    while (empty() && _can_sleep)
    {
        // 空通知消费者等待
        _notify_consumer.wait();
    }

    if (_can_sleep)
    {
        // 保存队列头，用于返回出队元素
        Elem_type tmp = _queue.front();
        // 出队
        _queue.pop();
        // 通知生产者生产
        _notify_producer.notify_all();
        // 返回
        return tmp;
    }
    else
    {
        return nullptr;
    }
}

// 判空
bool Task_queue::empty() const
{
    return 0 == _queue.size();
}

// 判满
bool Task_queue::full() const
{
    return _queue_capacity == _queue.size();
}

// 唤醒沉睡进程
void Task_queue::wake_up()
{
    _can_sleep = false;
    _notify_consumer.notify_all();
}