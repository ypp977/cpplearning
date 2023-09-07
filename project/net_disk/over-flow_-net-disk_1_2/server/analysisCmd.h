#ifndef _ANALYSISCMD_H_
#define _ANALYSISCMD_H_

#include "msg.h"
#include "dir.h"
#include <func.h>

typedef enum {
  COMMAND_CD,
  COMMAND_LS,
  COMMAND_PWD,
  COMMAND_MKDIR,
  COMMAND_RMDIR,
  COMMAND_RM,
  COMMAND_GETFILE,
  COMMAND_PUTFILE,
  COMMAND_QUIT
} cmd_type;

typedef struct cmd_s
{
    char cmd[1024];
    char args[1024];
} cmd_t;

int analysis_cmd(const struct msg_t* msg, int netfd, dirStack_t* curDir);

 #endif
