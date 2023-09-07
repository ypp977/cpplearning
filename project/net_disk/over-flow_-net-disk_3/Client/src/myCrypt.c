#include "../Include/getCryptPasswd.h"

//编译时加上 -lcrypt
//passwd:用户密码    salt:盐值、
//返回加密生成的字符串(106位)
char* getCryptPasswd(char *passwd, char * salt){
    char* ret = crypt(passwd, salt);

    return ret;
}

/* int main(int argc, char* argv[]){ */
/*     char passwd[32] = {0}; */
/*     memcpy(passwd, argv[1], (ssize_t)sizeof(argv[1])); */
/*     char salt[21] = "$6$asxsddsfsfsfsfsf$"; */
/*     char* ret; */
/*     ret = getCryptPasswd(passwd, salt); */
/*     puts(ret); */

/*     return 0; */
/* } */

