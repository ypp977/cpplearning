#include "client.h"

//#define MAX_ARGS 10
//int cmd_separa(char *cmd, char *argv) {
//  int argc = 0;
//  char *token = strtok(cmd," ");
//  while(token != NULL){
//      if(argc >= MAX_ARGS){
//	  fprintf(stderr,"Too many arguments\n");
//	  return -1;
//      }
//      token = strtok(NULL," ");
//  }
//  return argc;
//}

int cmd_separa(const char* path, char* tokens[], int* tokenNum) {

    char buf[1024] = { 0 };
    strncpy(buf, path, sizeof(buf));
    char* token; // 保存分割后的文件夹名
    char* saveptr; // 保存分割位置的指针

    token = strtok_r(buf, " ", &saveptr);
    *tokenNum = 0;
    while (token != NULL) {

        tokens[(*tokenNum)++] = token;
        token = strtok_r(NULL, " ", &saveptr);
    }
    return 0;
}
