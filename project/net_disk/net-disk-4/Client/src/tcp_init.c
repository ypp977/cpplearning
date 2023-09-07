#include "../Include/client.h"

int tcp_init(const char *ip,const  char *port){

    int sockfd = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in ser_addr;
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = inet_addr(ip);
    ser_addr.sin_port = htons(atoi(port));

    int conret = connect(sockfd,(struct sockaddr *)&ser_addr,sizeof(ser_addr));
    if(conret == -1){
	perror("connect");
	return -1;
    }
    return sockfd;
}
