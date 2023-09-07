#ifndef _FILE_H_
#define _FILE_H_

#define FILE_PIECE_SIZE 4096

struct file_info_t{
    char file_name[256];
    char md5_val[256];
    unsigned long file_size;
    unsigned long file_offset;
};

// 客户端执行get指令, path为存放文件的文件夹路径，非文件路径，成功为0，失败-1
int get_file(int socket_fd, const char *path);
// 客户端执行put指令，file_path为文件路径，成功为0，失败-1
int put_file(int socket_fd, const char *file_path);
int send_file(const int socket_fd, const int fd, struct file_info_t *file_info);
int recv_file(const int socket_fd, const int fd, struct file_info_t *file_info);


#endif

