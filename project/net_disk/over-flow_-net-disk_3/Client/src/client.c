#include "../Include/client.h"

int main(int argc,char* argv[]){
    // read config
    FILE* fp = fopen("client.conf", "r");
    char ip[256] = { 0 };
    char port[256] = { 0 };

    fscanf(fp, "%s %s", ip, port);
    fclose(fp);

    int sockfd = tcp_init(ip, port);
    ERROR_CHECK(sockfd,-1,"连接服务器失败");

    client_work(sockfd);
    close(sockfd);
    return 0;
}

