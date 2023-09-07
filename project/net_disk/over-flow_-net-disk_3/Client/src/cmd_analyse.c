#include "../Include/client.h"

int cmd_analyse(const char *cmd){
    if(strcmp(cmd,"cd") == 0){
	return COMMAND_CD;
    }
    if(strcmp(cmd,"ls") == 0){
	return COMMAND_LS;
    }
    if(strcmp(cmd,"pwd") == 0){
	return COMMAND_PWD;
    }
    if(strcmp(cmd,"mkdir") == 0){
	return COMMAND_MKDIR;
    }
    if(strcmp(cmd,"rmdir") == 0){
	return COMMAND_RMDIR;
    }
    if(strcmp(cmd,"rm") == 0){
	return COMMAND_RM;
    }
    if(strcmp(cmd,"get") == 0){
	return COMMAND_GETFILE;
    }
    if(strcmp(cmd,"put") == 0){
	return COMMAND_PUTFILE;
    }
    if(strcmp(cmd,"quit") == 0 || strcmp(cmd,"3") == 0){
	return COMMAND_QUIT;
    }
    if(strcmp(cmd,"1") == 0 || strcmp(cmd,"login") == 0){
	return COMMAND_LOGIN;
    }
    if(strcmp(cmd,"2") == 0 || strcmp(cmd,"register") == 0){
	return COMMAND_REGISTER;
    }
    return -1;
}
