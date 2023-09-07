#ifndef __CLIENT___
#define __CLIENT___
#include <func.h>
#include "file.h"
#include "msg.h"
enum {
  COMMAND_CD,
  COMMAND_LS,
  COMMAND_PWD,
  COMMAND_MKDIR,
  COMMAND_RMDIR,
  COMMAND_RM,
  COMMAND_GETFILE,
  COMMAND_PUTFILE,
  COMMAND_QUIT
};

typedef struct user_s{
    char name[32];
    char passwd[1000];
}user_t;

typedef struct cmd_s{
    char cmd[1024];
    char args[1024];
}cmd_t;

int tcpInit(char *ip,char *port);
void client_work(const int sockfd);
int cmd_separa(const char *path,char *tokens[],int *tokenNum);
int cmd_analyse(const char* cmd);
int is_cmd(int cmd_type,int argc);

#endif
