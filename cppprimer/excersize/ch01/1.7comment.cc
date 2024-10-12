// 练习1.7:
// 编写一个包含不正确的嵌套注释的程序，观察编译期返回的错误信息

#include <iostream>

int main()
{
    /*
    */
    */
    return 0;
}
// 报错如下所示
// 1.7comment.cc: In function ‘int main()’:
// 1.7comment.cc:7:6: error: expected primary-expression before ‘/’ token
//     7 |     */
//       |      ^
// 1.7comment.cc:8:5: error: expected primary-expression before ‘return’
//     8 |     return 0;
//       |     ^~~~~~