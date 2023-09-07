#ifndef __TCP_INIT__
#define __TCP_INIT__
//与服务器建立tcp连接
//ip 服务器ip地址
//port 服务器端口号
//失败返回-1
//成功返回socketfd
int tcp_init(const char *ip,const char *port);
#endif
