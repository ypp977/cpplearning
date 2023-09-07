#include "client.h"

void client_work(const int sockfd){

    char output[102400] = {0};
    user_t userInfo;
    char *passwd;
    struct msg_t msg;
    char *retmsg[1024];
    

    //登录
    //输入用户名
    printf(">username:");
    fgets(userInfo.name,sizeof(userInfo.name),stdin);
    //把末尾的\n替换为\0
    userInfo.name[strcspn(userInfo.name,"\n")] = '\0';
    //输入密码
    passwd = getpass(">passwd:");
    strncpy(userInfo.passwd,passwd,sizeof(userInfo.passwd));
    userInfo.passwd[sizeof(userInfo.passwd) - 1] = '\0';

    int verify = 1;
    while(verify){
	//初始化数据包
	init_msg(&msg);
	set_msgtype(&msg,U_INFO_T);
	set_msgsize(&msg,sizeof(userInfo));
	set_msgdata(&msg,&userInfo);
	//发送用户数据包
	send_msg(sockfd,&msg);
	//接收校验结果
	bzero(output,sizeof(output));

	int ret = recv_msg(sockfd,&msg);
	if(ret == -1){
	    printf("server is closed!\n");
	    return ;
	}
	//提取返回数据并输出
	get_msgdata(&msg,output);
	printf("%s\n",output);

	if(strcmp(output,"welcome!") == 0){
	    verify = 0;
	}else{
	    return;
	    //服务端未做多次判断，暂时放弃
	    printf("%s\n",output);

	    printf(">username:");
	    fgets(userInfo.name,sizeof(userInfo.name),stdin);
	    userInfo.name[strcspn(userInfo.name,"\n")] = '\0';

	    passwd = getpass(">passwd:");
	    strncpy(userInfo.passwd,passwd,sizeof(userInfo.passwd));
	    userInfo.passwd[sizeof(userInfo.passwd) - 1] = '\0';
	}
    }

    char input[1024];
    printf("[%s @-->]$ ",userInfo.name);
    fflush(stdout);

    fd_set rdset;
    while(1){
	FD_ZERO(&rdset);
	FD_SET(STDIN_FILENO,&rdset);
	FD_SET(sockfd,&rdset);

	int sret = select(sockfd + 1,&rdset,NULL,NULL,NULL);
	if(FD_ISSET(STDIN_FILENO,&rdset)){
	    char cmd[1024]={0};
	    cmd_t cmdInfo;

	    bzero(&cmdInfo,sizeof(cmd_t));
	    sret = read(STDIN_FILENO,cmd,sizeof(cmd));
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
	    //判断命令是否合法
	    if(!is_cmd(cmd_type,argc)){
		printf("Invalid command\n");
		printf("[%s @-->]$ ",userInfo.name);
		fflush(stdout);
		continue;
	    }

	    if(cmd_type == COMMAND_QUIT){
		printf("bye\n");
		return;
	    }

	    if(cmd_type == COMMAND_GETFILE){
		//初始化数据包
		init_msg(&msg);
		set_msgsize(&msg,sizeof(cmdInfo));
		set_msgdata(&msg,&cmdInfo);

		send_msg(sockfd,&msg);

		int ret = recv_file(sockfd,"./");
		if(ret == -1){
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
		int ret = send_file(sockfd,argv[1]); 
		if(ret == -1){
		    continue;
		}
		//printf("[%s @-->]$ ",userInfo.name);
		//fflush(stdout);
	    }
	    else {
		//初始化数据包
		init_msg(&msg);
		set_msgsize(&msg,sizeof(cmdInfo));
		set_msgdata(&msg,&cmdInfo);
		send_msg(sockfd,&msg);
	    }
	}
	if(FD_ISSET(sockfd,&rdset)){
	    //printf("[%s @-->]$ ",userInfo.name);

	    bzero(output,sizeof(output));
	    init_msg(&msg);

	    int ret = recv_msg(sockfd,&msg);
	    get_msgdata(&msg,output);
	    if(ret == -1){
		break;
	    }

	    printf("%s\n",output);
	    printf("[%s @-->]$ ",userInfo.name);
	    fflush(stdout);
	}
    } 
}

