#include "client.h"
int tcpInit(char *ip,char *port){

    //与服务器建立连接
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    //初始化
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = inet_addr(ip);

    int ret = connect(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(ret == -1){
	perror("connect");
	return -1;
    }
    return sockfd;
}
