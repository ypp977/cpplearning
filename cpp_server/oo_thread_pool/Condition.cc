#include "Condition.h"
#include "Mutex_lock.h"
#include <stdio.h>

Condition::Condition(Mutex_lock &mutex)
    : _mutex(mutex)
{
    int ret = pthread_cond_init(&_cond, nullptr);
    if (ret)
    {
        perror("init cond");
    }
}

Condition::~Condition()
{
    int ret = pthread_cond_destroy(&_cond);
    if (ret)
    {
        perror("destory cond");
    }
}

// 等待
void Condition::wait()
{
    int ret = pthread_cond_wait(&_cond, _mutex.get_lock_ptr());
    if (ret)
    {
        perror("wait");
        return;
    }
}

// 通知
void Condition::notify()
{
    int ret = pthread_cond_signal(&_cond);
    if (ret)
    {
        perror("notify");
        return;
    }
}

// 通知所有
void Condition::notify_all()
{
    int ret = pthread_cond_broadcast(&_cond);
    if (ret)
    {
        perror("notify_all");
        return;
    }
}
