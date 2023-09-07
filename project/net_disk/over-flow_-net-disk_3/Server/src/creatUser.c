#include "../Include/createUser.h"

//返回 -1:连接或执行SQL错误  -2：存在相同用户名  0：创建新用户成功

int create_user(char* username, char* upasswd, MYSQL * mysql){     //用户名，密码
    /* MYSQL * mysql = mysql_init(NULL); */
    /* MYSQL * ret = mysql_real_connect(mysql, "localhost", "root", "root", "overflow_netdisk", 0, NULL, 0); */

    /* //建立连接失败 */
    /* if(ret == NULL){ */
    /*     fprintf(stderr, "mysql_real_connect: %s\n", mysql_error(mysql)); */
    /*     return -1; */
    /* } */

//--------------------------------------
    
//查询是否存在同名用户
    char sql[1024] = {0};
    sprintf(sql, "select * from user where username = \'%s\';", username);
    int qret = mysql_query(mysql, sql);

    //查询失败
    if(qret != 0){
        fprintf(stderr, "mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }

    //获取查询结果
    MYSQL_RES* res = mysql_store_result(mysql);

    //存在相同用户名
    if(mysql_fetch_row(res) != NULL){
        mysql_free_result(res);
        return -2;
    }
    mysql_free_result(res);
    //----------------------------------------------

    //生成salt值
    char salt[16] = {0};
    int flag;

    srand(time(NULL));

    for(int i = 0; i < SALT_LEN; i++){
        flag = rand()%3;
        switch(flag){
        case 0:
            salt[i] = rand()%26 + 'a';
            break;
        case 1:
            salt[i] = rand()%26 + 'A';
            break;
        case 2:
            salt[i] = rand()%10 + '0';
            break;
        }
    }

    char retSalt[21] = {0};
    strncat(retSalt, "$6$", 3);
    strncat(retSalt, salt, 16);
    strncat(retSalt, "$", 1);

    //插入新用户
    bzero(sql, sizeof(sql));
    char *cryptPwd = getCryptPasswd(upasswd, retSalt);
    sprintf(sql, "insert into user (username, salt, password) values(\'%s\', \'%s\', \'%s\');", username, retSalt, cryptPwd);

    qret = mysql_query(mysql, sql);

    if(qret != 0){
        fprintf(stderr, "mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
    /* mysql_close(mysql); */
    return 0;
}

/* int main(int argc, char* argv[]){ */

/*     return 0; */
/* } */

