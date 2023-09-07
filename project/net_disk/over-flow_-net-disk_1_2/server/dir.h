#ifndef __DIR_HEAD__
#define __DIR_HEAD__
#include<func.h>
#define HOMEPATH "diruser/over"
//当前工作目录
#define BUFSIZE 4096

typedef struct dirNode_s{
    char dirName[4096]; 
    struct dirNode_s* next;
}dirNode_t;

typedef struct dirStack_s{
    dirNode_t* top;
    int length;
}dirStack_t;
//初始化
int initDir(dirStack_t* pdir);

//增
int addDir(dirStack_t* pdir, char* dirName);
//删
int delDir(dirStack_t* pdir);
int printStack(dirStack_t* pdir); // 调试函数
//提取路径
int getPath(dirStack_t* pdir, char* pathName);
int tokToPath(char* path, char* tokens[], int tokenNum);
//分词
int myStrtok(char*path, char* token[], int* tokenNum);
//cd
int myCd(dirStack_t* pdir,char *pathname);
//ls
int myLs(dirStack_t* pdir, char* dst);
//mkdir
int myMkdir(dirStack_t* pdir, char* pathName);
//rmdir
int myRmdir(dirStack_t* pdir, char* pathName);
//myRemove
//0表示删除成功
int myRemove(dirStack_t* pdir, char* fileName);

#endif
