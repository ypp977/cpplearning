#ifndef __DIR_HEAD__
#define __DIR_HEAD__
#include<func.h>
#include <mysql/mysql.h>
//当前工作目录
#define BUFSIZE 4096
typedef struct dirNode_s{
     char dirName[BUFSIZE/2];
     struct dirNode_s* next;
}dirNode_t;
typedef struct dirStack_s{
    dirNode_t* top;
    int length;
}dirStack_t;
typedef struct userCwd_s{
    int cwd;//指向MYSQL里的ID
    char user[255];//用户名
    dirStack_t dirstack;
}userCwd_t;

//初始化
int initCwd(userCwd_t* pUcwd, const char* username);

//增
int addDir(dirStack_t* pdir, char* dirName);
//删
int delDir(dirStack_t* pdir);
int printStack(dirStack_t* pdir); // 调试函数
//提取路径
int getPath(const dirStack_t* pdir, char* pathName);

//分词
int myStrtok(char*path, char* token[], int* tokenNum);

//cd
//返回-1表示非法无权限访问或该目录路径不存在
int myCd(userCwd_t* pUcwd, MYSQL* mysql, const char *dst);

//ls
//dst为传入传出参数
//一般无异常返回
int myLs(const userCwd_t* pUcwd, MYSQL* mysql, char* dst);

//pwd
//dst为传入传出参数
//一般无异常返回
int myPwd(const userCwd_t* pUcwd, char* pwd);

//mkdir
//返回-1表示参数不合法或无权限访问
int myMkdir(const userCwd_t* pUcwd, MYSQL* mysql,const char* pathName);

//rmdir
int myRmdir(userCwd_t* pUcwd, MYSQL* mysql,const char* pathName);

#endif
