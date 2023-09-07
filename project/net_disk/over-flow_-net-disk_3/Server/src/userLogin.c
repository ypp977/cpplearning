#include "../Include/userLogin.h"

//-1：连接失败或其他数据库错误  0：登录成功  1 用户名或者密码错误
int userLogin(char * username, char* passwd, MYSQL * mysql){
    /* MYSQL * mysql = mysql_init(NULL); */
    /* MYSQL * ret = mysql_real_connect(mysql, "localhost", "root", "root", "overflow_netdisk", 0, NULL, 0); */

    /* //建立连接失败 */
    /* if(ret == NULL){ */
    /*     fprintf(stderr, "mysql_real_connect: %s\n", mysql_error(mysql)); */
    /*     return -1; */
    /* } */

    //查询用户是否存在
    char sql[1024] = {0};
    sprintf(sql, "select * from user where username = \'%s\';", username);

    //查询失败
    int qret = mysql_query(mysql, sql);
    if(qret != 0) {
        fprintf(stderr, "mysql_real_query: %s\n", mysql_error(mysql));
        return -1;
    }
    //获取查询结果
    MYSQL_RES* res = mysql_use_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    //用户名不正确
    if(row == NULL){
        fprintf(stderr, "mysql_fetch_row:%s\n", mysql_error(mysql));
        return 1;
    }

    char * salt = row[2];
    char* cryptPasswd = row[3];
    char * makePasswd = crypt(passwd, salt);
    mysql_free_result(res);
    if(strcmp(cryptPasswd, makePasswd) == 0){
        return 0;
    }else {
        return 1;
    }
}


