#include "client.h"

int main(int argc,char* argv[]){
    
    int sockfd = tcpInit(argv[1],argv[2]);
    if(sockfd == -1){
	return -1;
    }
    client_work(sockfd);
    close(sockfd);
    return 0;
}

