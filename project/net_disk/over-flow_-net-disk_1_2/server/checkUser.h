#ifndef __CHECK_USER__
#define __CHECK_USER__

#include <shadow.h>
#include <func.h>
#include <crypt.h>
#include "msg.h"

typedef struct user_s{
    char name[32];
    char passwd[1000];
}user_t;

int checkUser(user_t * user, int netfd);

#endif
