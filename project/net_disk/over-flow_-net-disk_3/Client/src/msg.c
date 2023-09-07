#include <func.h>
#include "../Include/msg.h"

int init_msg(struct msg_t * msg){
    if(!msg){
        return -1;
    }
    bzero(msg, sizeof(struct msg_t));
    return 0;
}

int set_msgtype(struct msg_t *msg, msg_type type){
    if(!msg){
        return -1;
    }
    msg->type = type;
    return 0;
}

int set_msgsize(struct msg_t *msg, const unsigned int size){
    if(!msg){
        return -1;
    }
    msg->size = size;
    return 0;
}

int set_msgdata(struct msg_t *msg, const void * buf){
    if(!msg || !buf){
        return -1;
    }
    memcpy(msg->data, buf, msg->size);
    return 0;
}

int get_msgtype(const struct msg_t *msg, msg_type *type){
    if(!msg){
        return -1;
    }
    *type = msg->type;
    return 0;
}

int get_msgsize(const struct msg_t *msg, unsigned int *size){
    if(!msg){
        return -1;
    }
    *size = msg->size;
    return 0;
}

int get_msgdata(const struct msg_t *msg, void *buf){
    if(!msg || !buf){
        return -1;
    }
    memcpy(buf, msg->data, msg->size);
    return 0;
}

int send_msg(int socket_fd, struct msg_t * msg){
    if(!msg)
        return -1;

    int count = send(socket_fd, msg, sizeof(struct msg_t), 0);
    if(count == -1){
        return -1;
    }
    return 0;
}

int recv_msg(int socket_fd, struct msg_t *msg){
    if(!msg){
        return -1;
    }
    int count = recv(socket_fd, msg, sizeof(struct msg_t), MSG_WAITALL);
    if(count == 0 || count == -1 || count != sizeof(struct msg_t)){
        return -1;
    }
    return 0;
}
