// 练习1.4:
// 我们的程序使用加法运算符+来将两个数相加。编写程序使用乘法运算符*，来打印两个数的积

#include <iostream>

int main()
{
    std::cout << "input two num\n";
    int a = 0, b = 0;
    std::cin >> a >> b;
    std::cout << a * b << std::endl;
}