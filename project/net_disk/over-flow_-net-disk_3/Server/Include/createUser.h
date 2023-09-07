#ifndef __CREATE_USER__
#define __CREATE_USER__

#include "getCryptPasswd.h"
#include <mysql/mysql.h>
#define SALT_LEN 20 //定义盐值的长度

//创建新用户
int create_user(char* username, char* upasswd, MYSQL * mysql);

#endif
