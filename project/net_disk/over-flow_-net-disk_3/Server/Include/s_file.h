#ifndef _FILE_H_
#define _FILE_H_

#include <mysql/mysql.h>
#define FILE_PIECE_SIZE 4096

struct file_info_t{
    char file_name[256];
    char md5_val[256];
    unsigned long file_size;
    unsigned long file_offset;
};


// 服务端执行put指令，cur_path为当前目录路径，即PWD，成功返回0，失败返回-1
int put_file(int socket_fd, MYSQL *mysql, const char *user, char *cur_path);
// 服务端执行get指令，file_path为虚拟文件路径，成功返回0，失败返回-1
int get_file(int socket_fd, MYSQL *mysql, const char *user, char *file_path);
int send_file(const int socket_fd, const int fd, struct file_info_t *file_info);
int recv_file(const int socket_fd, const int fd, struct file_info_t *file_info);


#endif

