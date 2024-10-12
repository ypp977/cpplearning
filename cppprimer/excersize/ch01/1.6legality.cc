// 练习1.6:
// 解释下面程序片段是否合法。
// std::cout << "The sum of " << v1;
//           << " and " << v2;
//           << " is " << v1 + v2 << std::endl;
// 如果程序是合法的，他输出什么？如果程序不合法，原因合在？应该如何修正？
// 不合法，第二、三条语句的输出运算符左侧无运算对象，有两种改法：
// 1.每条输出语句单独放一行:
// 	std::cout << "The sum of " << v1;
//     std::cout << " and " << v2;
//     std::cout << " is " << v1 + v2 << std::endl;
// 2.把他放在同一条语句中:
//     std::cout << "The sum of " << v1
//           << " and " << v2
//           << " is " << v1 + v2 << std::endl;