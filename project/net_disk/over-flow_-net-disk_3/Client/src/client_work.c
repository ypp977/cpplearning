#include "../Include/client.h"
#include "../Include/client_work.h"

void client_work(const int sockfd){
    cmd_t cmdInfo;
    struct msg_t msg;
    user_t userInfo;    

    user_login(&cmdInfo,sockfd);

    memcpy(&userInfo,cmdInfo.args,sizeof(cmdInfo.args));

    printf("[%s @-->]$ ",userInfo.name);
    fflush(stdout);

    while(1){
	char cmd[1024]={0};

	bzero(&cmdInfo,sizeof(cmd_t));
	int sret = read(STDIN_FILENO,cmd,sizeof(cmd));
	//直接回车处理
	if(sret == 1){
	    printf("Invalid command\n");
	    printf("[%s @-->]$ ",userInfo.name);
	    fflush(stdout);
	    continue;
	}
	cmd[strlen(cmd) - 1] = '\0';

	int argc;//存放切割后的数量
	char* argv[10] = {0};
	cmd_separa(cmd,argv,&argc);
	strncpy(cmdInfo.cmd,argv[0],strlen(argv[0]));
	if(argc != 1){
	    strncpy(cmdInfo.args,argv[1],strlen(argv[1]));
	}

	int cmd_type = cmd_analyse(argv[0]);
	if(argc != 1){
	    if(cmd_type == COMMAND_PUTFILE){
		char tem[512];
		bzero(&tem,sizeof(tem));
		strncpy(tem,"./",2);
		strcat(tem,argv[1]);
		strcpy(argv[1],tem);
	    }
	    strncpy(cmdInfo.args,argv[1],strlen(argv[1]));
	}
	//判断命令是否合法
	if(!cmd_valid(cmd_type,argc)){
	    printf("Invalid command\n");
	    printf("[%s @-->]$ ",userInfo.name);
	    fflush(stdout);
	    continue;
	}

	if(cmd_type == COMMAND_QUIT){
	    memset(cmdInfo.cmd,'0',sizeof(cmdInfo.cmd));
	    strncpy(cmdInfo.cmd,"quit",sizeof(cmdInfo.cmd));

	    init_msg(&msg);
	    set_msgtype(&msg,COMMEND_T);
	    set_msgsize(&msg,sizeof(cmdInfo));
	    set_msgdata(&msg,&cmdInfo);
	    send_msg(sockfd,&msg);
	    printf("Exit success\n");
	    return;
	}

	if(cmd_type == COMMAND_GETFILE){
	    //初始化数据包
	    init_msg(&msg);
	    set_msgsize(&msg,sizeof(cmdInfo));
	    set_msgdata(&msg,&cmdInfo);

	    send_msg(sockfd,&msg);

	    int ret = get_file(sockfd,"../data");
	    if(ret == -1){
		printf("Unknown error occurred！\n");
		printf("[%s @-->]$ ",userInfo.name);
		fflush(stdout);
		fflush(stdin);
		continue;
	    }
	    printf("[%s @-->]$ ",userInfo.name);
	    fflush(stdout);
	}
	else  if(cmd_type == COMMAND_PUTFILE){
	    //初始化数据包
	    init_msg(&msg);
	    set_msgsize(&msg,sizeof(cmdInfo));
	    set_msgdata(&msg,&cmdInfo);
	    send_msg(sockfd,&msg);
	    int ret = put_file(sockfd,cmdInfo.args); 
	    if(ret == -1){
		printf("Unknown error occurred！\n");
		printf("[%s @-->]$ ",userInfo.name);
		fflush(stdout);
		continue;
	    }
	    printf("[%s @-->]$ ",userInfo.name);
	    fflush(stdout);
	}
	else {
	    //初始化数据包
	    init_msg(&msg);
	    set_msgsize(&msg,sizeof(cmdInfo));
	    set_msgdata(&msg,&cmdInfo);
	    send_msg(sockfd,&msg);

	    recv_msg(sockfd,&msg);
	    if(msg.size == 0){
		printf("Unknown error occurred！\n");
	    }else{
		printf("%s\n",msg.data);
		fflush(stdout);
	    }
	    printf("[%s @-->]$ ",userInfo.name);
	    fflush(stdout);
	}
    }
}


