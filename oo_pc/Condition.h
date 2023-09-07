#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "No_copy.h"
#include <pthread.h>

class Mutex_lock;

class Condition
    : No_copy
{
public:
    Condition(Mutex_lock &mutex);
    ~Condition();
    // 等待
    void wait();
    // 通知
    void notify();
    // 通知所有
    void notify_all();

private:
    Mutex_lock &_mutex;
    pthread_cond_t _cond;
};
#endif
