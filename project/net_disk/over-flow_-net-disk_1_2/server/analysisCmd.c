#include "analysisCmd.h"
#include "dir.h"
#include "file.h"

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
    if(strcmp(cmd,"quit") == 0){
	return COMMAND_QUIT;
    }
    return -1;
}

int analysis_cmd(const struct msg_t* msg, int netfd, dirStack_t* curDir)
{
    printf("analysis_cmd\n");

    cmd_t cmdinfo;
    bzero(&cmdinfo, sizeof(cmd_t));
    memcpy(&cmdinfo, msg->data, msg->size);
    char buf[4096] = {0};
    char buf2[2048] = {0};
    int flag = 0;

    struct msg_t ret;
    init_msg(&ret);

    switch (cmd_analyse(cmdinfo.cmd))
    {
    case COMMAND_CD:
        flag = myCd(curDir, cmdinfo.args);
        /* if(flag == -1) */
        /* { */
        /*     strcpy(buf, "CD error!"); */
        /*     set_msgtype(&ret, R_INFO_T); */
        /*     set_msgsize(&ret, strlen(buf) + 1); */
        /*     set_msgdata(&ret, buf); */
        /*     send_msg(netfd, &ret); */
        /* } */
        /* else if (flag == 0) */
        /* { */
        /*     set_msgtype(&ret, R_INFO_T); */
        /*     set_msgsize(&ret, 0); */
        /*     set_msgdata(&ret, buf); */
        /*     send_msg(netfd, &ret); */
        /* } */
        break;

    case COMMAND_LS:
        flag = myLs(curDir, buf);
        set_msgtype(&ret, R_INFO_T);
        set_msgsize(&ret, strlen(buf));
        set_msgdata(&ret, buf);
        send_msg(netfd, &ret);
        break;

    case COMMAND_PWD:
        flag = getPath(curDir, buf);
        set_msgtype(&ret, R_INFO_T);
        set_msgsize(&ret, strlen(buf));
        set_msgdata(&ret, buf);
        send_msg(netfd, &ret);
        break;

    case COMMAND_MKDIR:
        myMkdir(curDir, cmdinfo.args);
        break;

    case COMMAND_RMDIR:
        myRmdir(curDir, cmdinfo.args);
        break;

    case COMMAND_RM:
        myRemove(curDir, cmdinfo.args);
        break;

    case COMMAND_GETFILE:
        getPath(curDir, buf2);
        sprintf(buf, "%s/%s/%s", ".", buf2, cmdinfo.args);
        send_file(netfd, buf);
        break;

    case COMMAND_PUTFILE:
        getPath(curDir, buf2);
        sprintf(buf, "%s/%s/", ".", buf2);
        recv_file(netfd, buf);
        printf("Recv Over.\n");
        break;

    case COMMAND_QUIT:
        close(netfd);
        printf("One client logout!\n");
        return -1;
        break;
    }

    return 0;
}
