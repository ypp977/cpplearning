#include "client.h"

int is_cmd(const int cmd_type,const int argc){

    switch(cmd_type){
    case COMMAND_CD:
	return argc == 2;

    case COMMAND_LS:
	return argc == 2 || argc == 1;

    case COMMAND_PWD:
	return argc == 1;
	
    case COMMAND_MKDIR:
	return argc == 2;

    case COMMAND_RMDIR:
	return argc == 2;

    case COMMAND_RM:
	return argc == 2;

    case COMMAND_GETFILE:
	return argc == 2;

    case COMMAND_PUTFILE:
	return argc == 2;

    case COMMAND_QUIT:
	return argc == 1;

    default:
	return 0;
    }
}
