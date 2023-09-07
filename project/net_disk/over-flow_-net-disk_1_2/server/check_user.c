#include "checkUser.h"

//宏定义返回客户端的值
#define LOGIN_SUCCESS "welcome!"
#define LOGIN_FAILURE "passwd error!"

// gcc编译该程序时需要加入 -lcrypt
int checkUser(user_t * user, int netfd){
    struct msg_t msg;

    //获取服务端密码结构体
    struct spwd *localpwd = getspnam(user->name);

    //系统无对应用户名返回空
    if(localpwd == NULL){
        set_msgtype(&msg, R_INFO_T);
        set_msgsize(&msg, sizeof(LOGIN_FAILURE));
        set_msgdata(&msg, LOGIN_FAILURE);
        send_msg(netfd, &msg);
        return -1;
    }

    //获取用户加密后的暗码长度
    int len = strlen(localpwd->sp_pwdp);
    
    //从后往前寻找第一个$符号，确定salt值界限
    int idx = 0;
    for(int i = len - 1; i >= 0; i--){
        if(localpwd->sp_pwdp[i] == '$'){
            idx = i;
            break;
        }
    }
    
    //截取salt值
    char salt[1024] = {0}; 
    memcpy(salt, localpwd->sp_pwdp, idx + 1);
    
    //加密客户端传输过来的用户密码
    char* clientpwd = crypt(user->passwd, salt);
    
    //比较暗码
    init_msg(&msg);
    if(strcmp(clientpwd, localpwd->sp_pwdp) == 0){
        set_msgtype(&msg, R_INFO_T);
        set_msgsize(&msg, sizeof(LOGIN_SUCCESS));
        set_msgdata(&msg, LOGIN_SUCCESS);
        send_msg(netfd, &msg);
        return 1;
    } else{
        set_msgtype(&msg, R_INFO_T);
        set_msgsize(&msg, sizeof(LOGIN_FAILURE));
        set_msgdata(&msg, LOGIN_FAILURE);
        send_msg(netfd, &msg);
        return -1;
    }
} 
