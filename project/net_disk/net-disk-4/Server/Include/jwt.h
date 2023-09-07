#ifndef __JWT__
#define __JWT__
#include<func.h>
#include<l8w8jwt/encode.h>
#define BUFSIZE 4096 

//传入用户名user，获取token
int getJwt(const char* user, char* token);

#endif
