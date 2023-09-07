#include <func.h>
#include <errno.h>
#include <mysql/mysql.h>
#include "../Include/s_file.h"
#include "../Include/msg.h"
#include "../Include/md5.h"

// return 0 if success else return -1
static int init_file_info(struct file_info_t *file_info, const char *virtual_path, const char *real_path, int fd){
    if(!file_info || !virtual_path){
        return -1;
    }
    // 结构体置0
    bzero(file_info, sizeof(struct file_info_t));
    
    // 截断文件名放入结构体
    for(int i = strlen(virtual_path); i >= 0; --i){
        if(virtual_path[i] == '/'){
            strcpy(file_info->file_name, virtual_path+i+1);
            break;
        }
         else if(i == 0){
            strcat(file_info->file_name, virtual_path);
        }
    }

    // 计算md5值
    Compute_file_md5(real_path, file_info->md5_val);
    
    // 获取size
    file_info->file_size = lseek(fd, 0, SEEK_END);
    if(file_info->file_size == (unsigned long)-1){
        return -1;
    }
    lseek(fd, 0, SEEK_SET);
    
    // 偏移量置0
    file_info->file_offset = 0;
    return 0;
}

// 服务端get命令处理函数
int get_file(int socket_fd, MYSQL *mysql, const char *user, char *path){
    printf("Target Path : %s\n", path);
    struct msg_t msg;
    init_msg(&msg);
    if(!path){
        return -1;
    }
    
    
    // 查表获取物理文件名
    char sql_cmd[256];
    sprintf(sql_cmd, "SELECT md5 FROM fileInfo WHERE username = \'%s\' AND path = \'%s\' AND tomb = 0",
            user, path);

    int qret = mysql_query(mysql,sql_cmd);
    // 查表出错
    if(qret != 0){
        set_msgsize(&msg, sizeof("Error File"));
        set_msgdata(&msg, "Error File");
        send_msg(socket_fd, &msg);
        printf("Error: %s\n", mysql_error(mysql));
        return -1;
    }
    
    // 查表成功
    MYSQL_RES *result = mysql_store_result(mysql);
    
    // 表中不存在需要发送的文件，给客户端发送失败信息
    if(mysql_num_rows(result) == 0){
        set_msgsize(&msg, sizeof("Error File"));
        set_msgdata(&msg, "Error File");
        send_msg(socket_fd, &msg);
        return -1;
    }
    
    // 文件存在则转换物理地址
    MYSQL_ROW row;
    row = mysql_fetch_row(result);
    char real_path[256];
    sprintf(real_path, "../data/%s", row[0]);
    mysql_free_result(result);

    // 尝试打开文件，失败则告知客户端取消传送
    int fd = open(real_path, O_RDONLY);
    if(fd == -1){
        set_msgsize(&msg, sizeof("Error File"));
        set_msgdata(&msg, "Error File");
        send_msg(socket_fd, &msg);
        return -1;
    }

    // 打开成功
    struct file_info_t file_info;
    init_file_info(&file_info, path, real_path, fd);
    
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

// 服务端执行put指令
int put_file(const int socket_fd, MYSQL *mysql, const char *user, char *file_path){
    struct msg_t msg;
    init_msg(&msg);
    
    // 获取客户端发送文件信息
    recv_msg(socket_fd, &msg);
    if(msg.type != F_INFO_T){
        return -1;
    }
    // 若客户端出错则接收到客户端的错误信息
    if(!strcmp(msg.data, "Error File")){
        return -1;
    }
    
    struct file_info_t file_info;
    memcpy(&file_info, msg.data, msg.size);
    
    // 得到物理地址
    char path[512];
    sprintf(path, "../data/%s", file_info.md5_val);

    int fd = open(path, O_RDWR | O_CREAT, 0777);
    if(fd == -1){
        set_msgsize(&msg, sizeof("Error"));
        set_msgdata(&msg, "Error");
        send_msg(socket_fd, &msg);
        return -1;
    }
    
    file_info.file_offset = lseek(fd, 0, SEEK_END);

    // 回送偏移量信息
    set_msgsize(&msg, sizeof(file_info));
    set_msgdata(&msg, &file_info);
    send_msg(socket_fd, &msg);
    
    // 开始接收文件
    int ret = recv_file(socket_fd, fd, &file_info);

    // 接收成功
    if(ret == 0){
        // 判断MD5，不正确删除文件报错
        char file_md5[64];
        Compute_file_md5(path, file_md5);
        if(strcmp(file_info.md5_val, file_md5)){
            remove(path);
            return -1;
        }
        
        char sql_cmd[512];
        // 获取所在文件虚拟路径
        char v_file_path[256] = {0};

        if(strcmp(file_path, "/")){
            sprintf(v_file_path, "%s/%s", file_path, file_info.file_name);
        }else{
            sprintf(v_file_path, "%s%s", file_path, file_info.file_name);
        }
        printf("V_PATH : %s\n", v_file_path);
        // 查询表内是否存在文件
        sprintf(sql_cmd, 
                "SELECT md5 FROM fileInfo WHERE username = \'%s\' AND path = \'%s\'",
                user, v_file_path);

        int qret = mysql_query(mysql,sql_cmd);
        if(qret != 0){
            printf("Error: %s\n", mysql_error(mysql));
            return -1;
        }
        MYSQL_RES *result = mysql_store_result(mysql);

        int flag = mysql_num_rows(result);
        mysql_free_result(result);
        // 存在表，则更新表项
        if(flag){
            sprintf(sql_cmd,
                    "UPDATE fileInfo SET md5 = \'%s\', tomb = 0 "
                    "WHERE username = \'%s\' AND path = \'%s\'",
                    file_info.md5_val, user,v_file_path);
            qret = mysql_query(mysql, sql_cmd);
            if(qret != 0){
                printf("Error: %s\n", mysql_error(mysql));
                return -1;
            }
        }
        // 不存在，插入表项
        else{
            char pre_id[10];
            if(strcmp(file_path, "/"))
            {    sprintf(sql_cmd,
                         "SELECT id FROM fileInfo "
                         "WHERE username = \'%s\' AND path = \'%s\'",
                         user, file_path);
            qret = mysql_query(mysql, sql_cmd);
            if(qret != 0){
                printf("Error: %s\n", mysql_error(mysql));
                return -1;
            }
            result = mysql_store_result(mysql);

            MYSQL_ROW row;
            row = mysql_fetch_row(result);
            strcpy(pre_id, row[0]);
            mysql_free_result(result);
            // row中存储父目录id
            }else{
                strcpy(pre_id, "-1");
            }
            // 插入表项
            sprintf(sql_cmd,
                    "INSERT INTO fileInfo "
                    "(filename, type, pre_id, username, path, tomb, md5) "
                    "VALUES"
                    "(\'%s\', 'f', %s, \'%s\', \'%s\', 0, \'%s\')",
                    file_info.file_name,pre_id,user,v_file_path,file_info.md5_val);

            qret = mysql_query(mysql, sql_cmd);
            if(qret != 0){
                printf("Error: %s\n", mysql_error(mysql));
                return -1;
            }
        }
        return 0;
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
