#include <func.h>
#include <errno.h>
#include "../Include/c_file.h"
#include "../Include/msg.h"
#include "../Include/md5.h"

// return 0 if success else return -1
static int init_file_info(struct file_info_t *file_info, const char *path, int fd){
    if(!file_info || !path){
        return -1;
    }
    // 结构体置0
    bzero(file_info, sizeof(struct file_info_t));
    
    // 截断文件名放入结构体
    for(int i = strlen(path); i >= 0; --i){
        if(path[i] == '/'){
            strcpy(file_info->file_name, path+i+1);
            break;
        }
         else if(i == 0){
            strcpy(file_info->file_name, path);
        }
    }

    // 计算md5值
    Compute_file_md5(path, file_info->md5_val);
    
    // 获取size
    file_info->file_size = lseek(fd, 0, SEEK_END);
    if(file_info->file_size == -1){
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    
    // 偏移量置0
    file_info->file_offset = 0;
    return 0;
}

// 客户端put文件
int put_file(int socket_fd, const char *path){
    struct msg_t msg;
    init_msg(&msg);
    if(!path){
        return -1;
    }

    // 尝试打开文件，失败则告知服务端取消传送
    int fd = open(path, O_RDWR);
    if(fd == -1){
        set_msgsize(&msg, sizeof("Error File"));
        set_msgdata(&msg, "Error File");
        send_msg(socket_fd, &msg);
        printf("Opne File Error.\n");
        return -1;
    }
    printf("File Opened.\n");

    struct file_info_t file_info;
    init_file_info(&file_info, path, fd);
    
    set_msgtype(&msg, F_INFO_T);
    set_msgsize(&msg, sizeof(file_info));
    set_msgdata(&msg, &file_info);
    
    // 第一次给服务端发送文件描述信息
    send_msg(socket_fd, &msg);

    // 回收服务端确认文件信息
    recv_msg(socket_fd, &msg);
    if(msg.type != F_INFO_T){
        return -1;
    }
    if(!strcmp(msg.data, "Error")){
        return -1;
    }

    memcpy(&file_info, msg.data, sizeof(file_info));
    
    printf("Start Send.\n");
    int ret = send_file(socket_fd, fd, &file_info);

    return ret;
}

int send_file(const int socket_fd, const int fd, struct file_info_t *file_info){
    struct msg_t smsg;
    
    // 获取发送偏移量
    if(file_info->file_offset)
        lseek(fd, file_info->file_offset, SEEK_SET);
    unsigned long sum_size = file_info->file_size - file_info->file_offset;
    
    // 初始化发送信息
    init_msg(&smsg);
    set_msgtype(&smsg, FILE_T);
    
    // 判断是否触发mmap模式
    if(file_info->file_size >= 100 * 1024 * 1024)
    {
        char *start;
        start = (char *)mmap(NULL, file_info->file_size, PROT_READ, MAP_SHARED ,fd , 0);
        
        char *cur;
        cur = start + file_info->file_offset;
        while(sum_size){
            if(sum_size > FILE_PIECE_SIZE)
            {
                smsg.size = FILE_PIECE_SIZE;
                sum_size -= FILE_PIECE_SIZE;
            }else{
                smsg.size = sum_size;
                sum_size = 0;
            }
            memcpy(smsg.data, cur, smsg.size);
            cur += smsg.size;
            send_msg(socket_fd, &smsg);
        }
        munmap(start, file_info->file_size);
    }
    else
    {
        while(sum_size){
            if(sum_size > FILE_PIECE_SIZE)
            {
                smsg.size = FILE_PIECE_SIZE;
                sum_size -= FILE_PIECE_SIZE;
            }else{
                smsg.size = sum_size;
                sum_size = 0;
            }
            read(fd, smsg.data, smsg.size);
            send_msg(socket_fd, &smsg);
        }
    }
    close(fd);
    return 0;
}

int get_file(const int socket_fd, const char *path){
    struct msg_t msg;
    init_msg(&msg);
    recv_msg(socket_fd, &msg);
    if(msg.type != F_INFO_T){
        return -1;
    }

    if(!strcmp(msg.data, "Error File")){
        return -1;
    }
    
    struct file_info_t file_info;
    memcpy(&file_info, msg.data, msg.size);
    
    char file_path[256]={0};
    strcpy(file_path, path);
    if(file_path[strlen(path)] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_info.file_name);

    int fd = open(file_path, O_WRONLY | O_CREAT, 0777);
    if(fd == -1){
        set_msgsize(&msg, sizeof("Error"));
        set_msgdata(&msg, "Error");
        send_msg(socket_fd, &msg);
        return -1;
    }
    
    file_info.file_offset = lseek(fd, 0, SEEK_END);

    set_msgsize(&msg, sizeof(file_info));
    set_msgdata(&msg, &file_info);
    send_msg(socket_fd, &msg);
    
    int ret = recv_file(socket_fd, fd, &file_info);

    if(ret == 0){
        char file_mad5[64];
        Compute_file_md5(file_path, file_mad5);
        if(strcmp(file_info.md5_val, file_mad5)){
            remove(file_path);
            ret = -1;
        }
    }

    return ret; 
}

int recv_file(const int socket_fd, const int fd, struct file_info_t *file_info){
    struct msg_t rmsg;
    init_msg(&rmsg);

    unsigned long trans_size = file_info->file_size - file_info->file_offset;
    if(trans_size == 0)
    {
        return 0;
    }


    if(file_info->file_size >= 100 * 1024 * 1024){
        ftruncate(fd, file_info->file_size);
        char *start;
        start = (char *)mmap(NULL, file_info->file_size, PROT_WRITE, MAP_SHARED, fd, 0);
        char * cur = start + file_info->file_offset;

        while(!recv_msg(socket_fd, &rmsg)){
            if(rmsg.type == FILE_T){
                memcpy(cur, rmsg.data, rmsg.size);
                cur += rmsg.size;
                trans_size -= rmsg.size;
                if(trans_size <= 0)
                    break;
            }
        }
        munmap(start, file_info->file_size);
        ftruncate(fd, file_info->file_size - trans_size);
    }else{
        while(!recv_msg(socket_fd, &rmsg)){
            if(rmsg.type == FILE_T)
            {
                write(fd, rmsg.data, rmsg.size);
                trans_size -= rmsg.size;
                if(trans_size <= 0)
                    break;
            }
        }
    }
    close(fd);
    return 0;
}
