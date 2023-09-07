#include "../Include/client.h"

void user_input(user_t *userInfo){
    char *passwd;
    //登录
    //输入用户名
    printf(">username:");
    fgets(userInfo->name,sizeof(userInfo->name),stdin);
    //把末尾的\n替换为\0
    userInfo->name[strcspn(userInfo->name,"\n")] = '\0';
    //输入密码
    passwd = getpass(">passwd:");
    strncpy(userInfo->passwd,passwd,sizeof(userInfo->passwd));
    userInfo->passwd[sizeof(userInfo->passwd) - 1] = '\0';
}

void user_login(cmd_t *cmd_info,longMsg_t *longMsg,const int sockfd){
    struct msg_t msg;
    int ret;
    int verify = 1;
    int num = 3;
    user_t userInfo;

    printf("%-15s%-15s%-15s\n","1 login","2 register","3 quit");
    fgets(cmd_info->cmd, sizeof(cmd_info->cmd), stdin); // 读取命令
    cmd_info->cmd[strcspn(cmd_info->cmd, "\n")] = '\0'; // 去掉换行符;

    while(verify){

	ret = cmd_analyse(cmd_info->cmd);
	if(ret == COMMAND_QUIT){
	    //退出
	    memset(cmd_info->args,'0',sizeof(cmd_info->args));
	    memset(cmd_info->cmd,'0',sizeof(cmd_info->cmd));
	    strncpy(cmd_info->cmd,"quit",sizeof(cmd_info->cmd));

	    init_msg(&msg);
	    set_msgtype(&msg,COMMEND_T);
	    set_msgsize(&msg,sizeof(msg));
	    set_msgdata(&msg,cmd_info);

	    send_msg(sockfd,&msg);
	    exit(0);

	}
	if(ret == COMMAND_REGISTER){
	    bzero(&userInfo,sizeof(userInfo));
	    //注册
	    user_input(&userInfo);
	    int ret = strcmp(userInfo.name,"");
	    if(ret == 0){
		printf("name or password cannot be empty!\n");
		user_input(&userInfo);
	    }

	    memset(cmd_info->cmd,'0',sizeof(cmd_info->cmd));
	    strcpy(cmd_info->cmd,"register");
	    memcpy(cmd_info->args,&userInfo,sizeof(user_t));

	    /* printf("cmd:%s\n",cmd->cmd); */
	    /* //test */
	    /* user_t test; */
	    /* memcpy(&test,cmd->args,sizeof(cmd->args)); */
	    /* printf("testname:%spasswd%s\n",test.name,test.passwd); */
	    /* //testend */
	    init_msg(&msg);
	    set_msgtype(&msg,U_INFO_T);
	    set_msgsize(&msg,sizeof(cmd_t));
	    set_msgdata(&msg,cmd_info);

	    send_msg(sockfd,&msg);

	    init_msg(&msg);
	    recv_msg(sockfd,&msg);
	    if(msg.size == 0){
		printf("register failed!\n");
		continue;
	    }else{
		printf("sucess\n");
		memcpy(longMsg,msg.data,msg.size);
		
		verify = 0;
	    }
	}
	else if(ret == COMMAND_LOGIN){
	    bzero(&userInfo,sizeof(userInfo));
	    //登录
	    user_input(&userInfo);
	    int ret = strcmp(userInfo.name,"");
	    if(ret == 0){
		printf("name or password cannot be empty!\n");
		user_input(&userInfo);
	    }

	    memset(cmd_info->cmd,'0',sizeof(cmd_info->cmd));
	    strncpy(cmd_info->cmd,"login",sizeof(cmd_info->cmd));
	    memcpy(cmd_info->args,&userInfo,sizeof(cmd_info->args));

	    init_msg(&msg);
	    set_msgtype(&msg,U_INFO_T);
	    set_msgsize(&msg,sizeof(cmd_t));
	    set_msgdata(&msg,cmd_info);

	    send_msg(sockfd,&msg);

	    init_msg(&msg);
	    recv_msg(sockfd,&msg);
	    if(msg.size == 0){
		printf("wrong username or password!\n\n");
		num--;
		if(num == 0){
		    printf("Exceeded the limit of errors. Program will automatically exit.\n\n");
		    exit(0);
		}
	    }else{
		memcpy(longMsg,msg.data,msg.size);
		//printf("username:%s\ntoken:%s\nnetfd:%d\n",longMsg->username,longMsg->token,longMsg->netfd);
		verify = 0;
	    }
	}else{
	    printf("Invalid command\n");
	    bzero(cmd_info,sizeof(cmd_info));
	    fgets(cmd_info->cmd, sizeof(cmd_info->cmd), stdin); // 读取命令
	    cmd_info->cmd[strcspn(cmd_info->cmd, "\n")] = '\0'; // 去掉换行符;
	    continue;
	}
    }
}
