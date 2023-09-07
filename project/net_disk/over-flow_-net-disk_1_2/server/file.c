#include <func.h>
#include "file.h"
#include "msg.h"
#include "log.h"

// return 0 if success else return -1
int init_file_info(struct file_info_t *file_info, const char *path, int fd){
    if(!file_info || !path){
        NETDISK_LOG_ERR("Invalid arguments.");
        return -1;
    }
    bzero(file_info, sizeof(struct file_info_t));
    strcpy(file_info->file_name, path);
    file_info->file_size = lseek(fd, 0, SEEK_END);
    if(file_info->file_size == -1){
        NETDISK_LOG_ERR("File No exist.");
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    file_info->file_offset = 0;
    return 0;
}

int send_file(const int socket_fd, const char * path){
    struct file_info_t file_info;
    struct msg_t smsg;
    
    init_msg(&smsg);
    set_msgtype(&smsg, F_INFO_T);
    int fd = open(path, O_RDONLY);
    if(fd == -1){
        set_msgsize(&smsg, sizeof("Error File"));
        set_msgdata(&smsg, "Error File");
        send_msg(socket_fd, &smsg);
        NETDISK_LOG_ERR("Invalid arguments.");
        return -1;
    }
    set_msgsize(&smsg, sizeof("Check"));
    set_msgdata(&smsg, "Check");
    send_msg(socket_fd, &smsg);
    
    init_file_info(&file_info, path, fd);
    init_msg(&smsg);
    set_msgtype(&smsg, F_INFO_T);
    set_msgsize(&smsg, sizeof(file_info));
    set_msgdata(&smsg, &file_info);
    send_msg(socket_fd, &smsg);

    struct msg_t rmsg;
    struct file_info_t check;
    recv_msg(socket_fd, &rmsg);
    if(rmsg.type != F_INFO_T){
        NETDISK_LOG_ERR("Trans Error.");
        return -1;
    }
    memcpy(&check, rmsg.data, sizeof(check));

    if(check.file_offset)
        lseek(fd, check.file_offset, SEEK_SET);
    unsigned long sum_size = check.file_size - check.file_offset;
    
    init_msg(&smsg);
    set_msgtype(&smsg, FILE_T);
    if(check.file_size >= 100 * 1024 * 1024)
    {
        char *start;
        start = (char *)mmap(NULL, check.file_size, PROT_READ, MAP_SHARED ,fd , 0);
        
        char *cur;
        cur = start + check.file_offset;
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
        munmap(start, check.file_size);
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
    NETDISK_LOG_INFO("Send File Done.");
    close(fd);
    return 0;
}

int recv_file(const int socket_fd, const char *path){
    struct msg_t rmsg;
    init_msg(&rmsg);
    recv_msg(socket_fd, &rmsg);
    if(rmsg.type != F_INFO_T){
        NETDISK_LOG_ERR("Trans Error.");
        return -1;
    }

    if(!strcmp(rmsg.data, "Error File")){
        NETDISK_LOG_ERR("File Error.");
        return -1;
    }

    init_msg(&rmsg);
    recv_msg(socket_fd, &rmsg);
    if(rmsg.type != F_INFO_T){
        NETDISK_LOG_ERR("Trans Error.");
        return -1;
    }

    struct file_info_t file_info;
    memcpy(&file_info, rmsg.data, rmsg.size);

    char file_path[256] = {0};
    strcat(file_path, path);
    for(int i = strlen(file_info.file_name); i >= 0; --i){
        if(file_info.file_name[i] == '/'){
            strcat(file_path, file_info.file_name+i);
            break;
        }
        else if(i == 0){
            strcat(file_path, file_info.file_name);
        }
    }

    int fd = open(file_path, O_RDWR | O_CREAT, 0777);
    if(fd == -1){
        NETDISK_LOG_ERR("File Open Error.");
        return -1;
    }

    file_info.file_offset = lseek(fd, 0, SEEK_END);


    struct msg_t smsg;
    init_msg(&smsg);
    set_msgtype(&smsg, F_INFO_T);
    set_msgsize(&smsg, sizeof(file_info));
    set_msgdata(&smsg, &file_info);
    send_msg(socket_fd, &smsg);

    unsigned long trans_size = file_info.file_size - file_info.file_offset;
    if(trans_size == 0)
    {
        NETDISK_LOG_INFO("Trans Done.");
        return 0;
    }


    if(file_info.file_size >= 100 * 1024 * 1024){
        ftruncate(fd, file_info.file_size);
        char *start;
        start = (char *)mmap(NULL, file_info.file_size, PROT_WRITE , MAP_SHARED, fd, 0);
        char * cur = start + file_info.file_offset;
        init_msg(&rmsg);
        while(!recv_msg(socket_fd, &rmsg)){
            printf("Trans Size: %ld\n", trans_size);
            if(rmsg.type == FILE_T){
                printf("Rmsg.size : %ul\n", rmsg.size);
                printf("Cur : %p\n", cur);
                memcpy(cur, rmsg.data, rmsg.size);
                cur += rmsg.size;
                trans_size -= rmsg.size;
                if(trans_size <= 0)
                    break;
                init_msg(&rmsg);
            }
        }       
        munmap(start, file_info.file_size);
        ftruncate(fd, file_info.file_size - trans_size);
        printf("Ftruncate Done.\n");
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
    NETDISK_LOG_INFO("File Recv Done.");
    close(fd);
    return 0;
}
