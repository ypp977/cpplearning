#include "Mutex_lock.h"
#include <stdio.h>

Mutex_lock::Mutex_lock()
{
    // 初始化锁
    int ret = pthread_mutex_init(&_mutex, nullptr);
    if (ret)
    {
        perror("Mutex_lock");
        return;
    }
}

Mutex_lock::~Mutex_lock()
{
    // 销毁锁
    int ret = pthread_mutex_destroy(&_mutex);
    if (ret)
    {
        perror("~Mutex_lock");
        return;
    }
}

void Mutex_lock::lock()
{
    // 上锁
    int ret = pthread_mutex_lock(&_mutex);
    if (ret)
    {
        perror("lock fail");
    }
}

void Mutex_lock::unlock()
{
    // 解锁
    int ret = pthread_mutex_unlock(&_mutex);
    if (ret)
    {
        perror("unlock fail");
    }
}

void Mutex_lock::try_lock()
{
    // 尝试上锁
    int ret = pthread_mutex_trylock(&_mutex);
    if (ret)
    {
        perror("try_lock fail");
    }
}

// 获取锁的裸指针
pthread_mutex_t *Mutex_lock::get_lock_ptr()
{
    return &_mutex;
}
