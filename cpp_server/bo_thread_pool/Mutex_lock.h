#ifndef __MUTEX_LOCK_H__
#define __MUTEX_LOCK_H__

#include "No_copy.h"
#include <pthread.h>
#include <stdio.h>

class Mutex_lock
    : No_copy
{
    friend class Auto_lock;

public:
    Mutex_lock();

    ~Mutex_lock();

    // 上锁
    void lock();

    // 解锁
    void unlock();

    // 尝试上锁
    void try_lock();

    // 获取锁的裸指针
    pthread_mutex_t *get_lock_ptr();

private:
    pthread_mutex_t _mutex;
};
// 利用RAII技术实现自动上锁解锁
class Auto_lock
{
public:
    Auto_lock(Mutex_lock &mutex)
        : _mutex(mutex)
    {
        _mutex.lock();
    }

    ~Auto_lock()
    {
        _mutex.unlock();
    }

private:
    Mutex_lock &_mutex;
};

#endif
