#ifndef _FILE_H_
#define _FILE_H_

#define FILE_PIECE_SIZE 4096

struct file_info_t{
    char file_name[256];
    unsigned long file_size;
    unsigned long file_offset;
};

int init_file_info(struct file_info_t *file_info, const char *path, int fd);
int send_file(const int socket_fd, const char *path);
int recv_file(const int socket_fd, const char *path);


#endif

