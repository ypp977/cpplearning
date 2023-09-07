#include "Work_thread.h"

Work_thread::Work_thread(Thread_pool &thread_pool)
    : _thread_pool(thread_pool)
{
}

Work_thread::~Work_thread()
{
}
