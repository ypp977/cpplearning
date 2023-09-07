#ifndef __USERLOGIN__
#define __USERLOGIN__

#include<crypt.h>
#include<shadow.h>
#include<mysql/mysql.h>
#include<func.h>

int userLogin(char * username, char* passwd, MYSQL * mysql);

#endif
