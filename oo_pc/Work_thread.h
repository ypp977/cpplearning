#ifndef __WORK_THREAD_H__
#define __WORK_THREAD_H__
#include "Thread.h"

class Thread_pool;
class Work_thread
    : public Thread
{
public:
    Work_thread(Thread_pool &thread_pool);
    ~Work_thread();

private:
    Thread_pool &_thread_pool;
};
#endif