#ifndef  __GETCRYPTPASSWD__
#define __GETCRYPTPASSWD__

#include<func.h>
#include<shadow.h>
#include<crypt.h> 

char* getCryptPasswd(char *passwd, char * salt);

#endif

