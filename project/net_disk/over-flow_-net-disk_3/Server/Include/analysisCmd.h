#ifndef _ANALYSISCMD_H_
#define _ANALYSISCMD_H_

#include "head.h"

typedef enum {
    COMMAND_CD,
    COMMAND_LS,
    COMMAND_PWD,
    COMMAND_MKDIR,
    COMMAND_RMDIR,
    COMMAND_RM,
    COMMAND_GETFILE,
    COMMAND_PUTFILE,
    COMMAND_QUIT,
    COMMAND_REGISTER,
    COMMAND_LOGIN,
} cmd_type;

typedef struct cmd_s
{
    char cmd[256];
    char args[1024];
} cmd_t;

typedef struct user_s
{
    char name[64];
    char password[960];
} user_t;

int analysis_cmd(const struct msg_t* msg, int netfd, MYSQL* mysql, userCwd_t* userCwd, user_t* userInfo);

#endif

