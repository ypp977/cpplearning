#include "Thread_pool.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

class My_task
{
public:
    void process()
    {
        ::srand(::clock());
        int num = rand() % 100;
        std::cout << "bo_thread_pool My_task num = " << num << std::endl;
    }
};

void test()
{
    // 创建任务指针
    std::unique_ptr<My_task> ptask(new My_task());

    // 创建线程池和任务队列
    Thread_pool thread_pool(4, 10);

    // 启动线程池
    thread_pool.start();

    // 循环添加任务
    int cnt = 20;
    while (cnt-- > 0)
    {
        thread_pool.add_task(std::bind(&My_task::process, ptask.get()));
        std::cout << "cnt = " << cnt << std::endl;
    }

    // 关闭线程池
    thread_pool.stop();
}

int main()
{
    test();
    return 0;
}
