// 练习1.2:
// 改写程序，让他返回-1.返回-1通常被当做程序错误的标识。
// 重新编译并运行你的程序，观察你的系统如何处理main返回的错误表示

#include <iostream>

int main()
{
    // return -1; // 表示程序执行错误 echo $? 结果为255
    return 0; // 表示程序执行正常 echo $? 结果为0
}