// 练习1.5:
// 我们将所有输出操作放在一条很长的语句中。
// 重写程序，将每个运算对象打印操作放在独立的语句中

#include <iostream>

int main()
{
    int a = 0,b = 0;

    std::cout << "Enter two numbers:";
    std::cout << std::endl;

    std::cin >> a;
    std::cin >> b;
    
    std::cout << "the product is: ";
    std::cout << a;
    std::cout << " * ";
    std::cout << b;
    
    std::cout << " = ";
    std::cout << a*b;
    std::cout << std::endl;

    return 0;
}