#include "../Include/client.h"

void *thread_function(void *args){
    thread_t *parg = (thread_t *)args;
    struct msg_t thread_msg;
    cmd_t thread_cmd;
    longMsg_t thread_long_msg;
    int ret;

    bzero(&thread_cmd,sizeof(thread_cmd));
    memcpy(&thread_cmd,parg->cmd_info,sizeof(thread_cmd));

    int thread_sockfd = tcp_init(parg->ip,parg->port);
    ERROR_CHECK(thread_sockfd,-1,"thrad_tcp_init");

    init_msg(&thread_msg);
    set_msgsize(&thread_msg,sizeof(thread_cmd));
    set_msgdata(&thread_msg,&thread_cmd);
    ret = send_msg(thread_sockfd,&thread_msg);


    recv_msg(thread_sockfd,&thread_msg);
    char tem[1024];
    bzero(tem,sizeof(tem));
    memcpy(tem,thread_msg.data,sizeof(tem));

    if(strcmp(thread_msg.data,"1") != 0){
	close(thread_sockfd);
	return 0;
    }
    //TODO
    //send 2rd
    memcpy(thread_long_msg.username,parg->longMsg.username,sizeof(thread_long_msg.username));
    memcpy(thread_long_msg.token,parg->longMsg.token,sizeof(thread_long_msg.token));
    memcpy(&thread_long_msg.netfd,&parg->longMsg.netfd,sizeof(thread_long_msg.netfd));

    init_msg(&thread_msg);
    set_msgsize(&thread_msg,sizeof(thread_long_msg));
    set_msgdata(&thread_msg,&thread_long_msg);
    ret = send_msg(thread_sockfd,&thread_msg);

    ret = recv_msg(thread_sockfd,&thread_msg);

    if(strcmp(thread_msg.data,"-1") == 0){
	close(thread_sockfd);
	return 0;
    }

    if(strcmp(thread_cmd.cmd,"put") == 0){

	ret = put_file(thread_sockfd,thread_cmd.args);
	if(ret == -1){
	    printf("\nUnknown error occurred！\n");
	}
	else{
	    printf("\nupload successful\n");
	}
	printf("[%s @-->]$ ",parg->longMsg.username);
	fflush(stdout);
    }
    else if(strcmp(thread_cmd.cmd,"get") == 0){
	int ret = get_file(thread_sockfd,"../data");
	if(ret == -1){
	    printf("\nUnknown error occurred！\n");
	}
	else{
	    printf("\ndownload successful\n");
	}
	printf("[%s @-->]$ ",parg->longMsg.username);
	fflush(stdout);
    }
    close(thread_sockfd);
    return 0;
}

void client_work(const int sockfd,const char *ip,const char *port){
    cmd_t cmdInfo;
    struct msg_t msg;
    user_t userInfo;
    char cmd[1024]={0};
    thread_t thread_args;
    pthread_t tid;
    longMsg_t longMsg;
    //登录
    user_login(&cmdInfo,&longMsg,sockfd);
    memcpy(&userInfo,cmdInfo.args,sizeof(cmdInfo.args));

    printf("[%s @-->]$ ",userInfo.name);
    fflush(stdout);

    fd_set rdset;
    while(1){
	FD_ZERO(&rdset);
	FD_SET(STDIN_FILENO,&rdset);
	FD_SET(sockfd,&rdset);

	int sret = select(sockfd + 1,&rdset,NULL,NULL,NULL);

	if(FD_ISSET(STDIN_FILENO,&rdset)){
	    bzero(&cmd,sizeof(cmd));
	    int sret = read(STDIN_FILENO,cmd,sizeof(cmd));
	    //直接回车处理
	    if(sret == 1){
		printf("Invalid command\n");
		bzero(&cmd,sizeof(cmd));
		printf("[%s @-->]$ ",userInfo.name);
		fflush(stdout);
		continue;
	    }
	    cmd[strlen(cmd) - 1] = '\0';

	    int argc;//存放切割后的数量
	    char* argv[10] = {0};
	    cmd_separa(cmd,argv,&argc);

	    bzero(&cmdInfo,sizeof(cmdInfo));
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
		bzero(&cmdInfo,sizeof(cmdInfo));
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
		bzero(&thread_args,sizeof(thread_args));
		memcpy(&thread_args.longMsg.netfd,&longMsg.netfd,sizeof(longMsg.netfd));
		memcpy(thread_args.longMsg.username,userInfo.name,sizeof(userInfo.name));
		memcpy(thread_args.longMsg.token,longMsg.token,sizeof(longMsg_t));
		memcpy(thread_args.cmd_info,&cmdInfo,sizeof(cmdInfo));
		memcpy(thread_args.ip,ip,sizeof(thread_args.ip));
		memcpy(thread_args.port,port,sizeof(thread_args.port));
		
		pthread_create(&tid,NULL,thread_function,&thread_args);
		//初始化数据包
		/* init_msg(&msg); */
		/* set_msgsize(&msg,sizeof(cmdInfo)); */
		/* set_msgdata(&msg,&cmdInfo); */

		/* send_msg(sockfd,&msg); */

		/* int ret = get_file(sockfd,"../data"); */
		/* if(ret == -1){ */
		/*     printf("Unknown error occurred！\n"); */
		/*     printf("[%s @-->]$ ",userInfo.name); */
		/*     fflush(stdout); */
		/*     fflush(stdin); */
		/*     continue; */
		/* } */
		/* printf("[%s @-->]$ ",userInfo.name); */
		/* fflush(stdout); */
	    }
	    else  if(cmd_type == COMMAND_PUTFILE){
		bzero(&thread_args,sizeof(thread_args));
		memcpy(&thread_args.longMsg.netfd,&longMsg.netfd,sizeof(longMsg.netfd));
		memcpy(thread_args.longMsg.username,userInfo.name,sizeof(userInfo.name));
		memcpy(thread_args.longMsg.token,longMsg.token,sizeof(longMsg_t));
		memcpy(thread_args.cmd_info,&cmdInfo,sizeof(cmdInfo));
		memcpy(thread_args.ip,ip,sizeof(thread_args.ip));
		memcpy(thread_args.port,port,sizeof(thread_args.port));
		//初始化数据包
		/* init_msg(&msg); */
		/* set_msgsize(&msg,sizeof(cmdInfo)); */
		/* set_msgdata(&msg,&cmdInfo); */

		pthread_create(&tid,NULL,thread_function,&thread_args);
		/* printf("args:%s\n",cmdInfo.args); */
		/* send_msg(sockfd,&msg); */
		/* int ret = put_file(sockfd,cmdInfo.args); */ 
		/* if(ret == -1){ */
		/*     printf("Unknown error occurred！\n"); */
		/*     printf("[%s @-->]$ ",userInfo.name); */
		/*     fflush(stdout); */
		/*     continue; */
		/* } */
		/* printf("[%s @-->]$ ",userInfo.name); */
		/* fflush(stdout); */
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
		bzero(&cmdInfo,sizeof(cmdInfo));

		printf("[%s @-->]$ ",userInfo.name);
		fflush(stdout);
	    }

	}
	if(FD_ISSET(sockfd,&rdset)){
	    int ret = recv(sockfd, &msg, sizeof(msg), MSG_WAITALL);
	    if(ret <= 0){
		printf("\nLink Closed\n");
		printf("Wating Thread...\n");
		pthread_join(tid, NULL);
		close(sockfd);
		exit(0);
	    }
	}

    }
}
