#ifndef _MSG_H_
#define _MSG_H_

typedef enum{
    COMMEND_T,
    FILE_T,
    R_INFO_T,
    F_INFO_T,
    U_INFO_T,
}msg_type;

struct msg_t{
    msg_type type; // Message type 1 cmd/ 0 file
    unsigned int size; // Message size
    char data[4096]; // Message data
};

int init_msg(struct msg_t *msg);
int set_msgtype(struct msg_t *msg, msg_type type);
int set_msgsize(struct msg_t *msg, const unsigned int size);
int set_msgdata(struct msg_t *msg, const void *buf);
int get_msgtype(const struct msg_t *msg, msg_type *type);
int get_msgsize(const struct msg_t *msg, unsigned int *size);
int get_msgdata(const struct msg_t *msg, void * buf);
int send_msg(int socket_fd, struct msg_t * msg);
int recv_msg(int socket_fd, struct msg_t * msg);


#endif
