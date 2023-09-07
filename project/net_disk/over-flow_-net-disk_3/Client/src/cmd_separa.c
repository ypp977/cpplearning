#include "../Include/client.h"

int cmd_separa(const char* cmd, char* tokens[], int* tokenNum) {

    char buf[1024] = { 0 };
    strncpy(buf, cmd, sizeof(buf));
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

