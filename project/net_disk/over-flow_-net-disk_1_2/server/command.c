#include "dir.h"

int myStrtok(char* path, char* tokens[], int* tokenNum)
{
    tokens[(*tokenNum)++] = strtok(path, "/");
    char* p;
    while((p = strtok(NULL,"/")) != NULL)
    {
        tokens[(*tokenNum)++] = p;
    }
    return 0;
}
 //初始化
int initDir(dirStack_t* pdir)
{
    bzero(pdir,sizeof(*pdir));
    pdir->top = (dirNode_t *)malloc(sizeof(dirNode_t));
    if(pdir->top == NULL)
    {
        return -1;
    }
    memcpy(pdir->top->dirName, HOMEPATH, sizeof(HOMEPATH));
    pdir->top->next = NULL;
    pdir->length = 1;//不一样
    return 0;
}

//增
int addDir(dirStack_t* pdir, char* dirName)
{
    dirNode_t* newDir = (dirNode_t *)malloc(sizeof(dirNode_t));
    if(newDir == NULL)
    {
        return -1;
    }
    memcpy(newDir->dirName, dirName, sizeof(newDir->dirName));
    newDir->next = pdir->top;
    pdir->top = newDir;
    ++(pdir->length);
    return 0;
}
//删
int delDir(dirStack_t* pdir)
{
    dirNode_t* tmp = pdir->top;
    pdir->top = pdir->top->next;
    free(tmp);
    --(pdir->length);
    return 0;
}
// 调试函数-打印当前栈的内容
int printStack(dirStack_t* pdir) {
    dirNode_t* p = pdir->top;
    while (p) {
        printf("curDir = %s\n", p->dirName);
        p = p->next;
    }
    return 0;
}
//提取路径
int getPath(dirStack_t* pdir, char* pathName)
{
    dirNode_t* tmp = pdir->top;
    int n = pdir->length;
    dirNode_t* arr = (dirNode_t *)calloc(n, sizeof(dirNode_t));
    if(arr == NULL)
    {
        return -1;
    }
    for(int i = n - 1; i >= 0 ; i--)
    {
        arr[i] = *tmp;
        tmp = tmp->next;
    }
    sprintf(pathName, "%s",arr[0].dirName);
    for(int i = 1; i < n ; i++)
    {
        sprintf(pathName, "%s/%s", pathName, arr[i].dirName);
    }

    return 0;
}

//cd
int myCd(dirStack_t* pdir, char* path)
{
    //检测
    char absPath[BUFSIZE] = {0};
    getPath(pdir, absPath);
   // snprintf(absPath,sizeof(absPath) + 1, "%s/%s",absPath,path);
   char tmpPath[BUFSIZE] = {0};
    snprintf(tmpPath,sizeof(tmpPath) + 1, "%s/%s",absPath,path);
   // sprintf(absPath, "%s/%s",absPath,path);
    struct stat st;
    if(stat(tmpPath, &st) != 0)
    {
        return -1;
    }
    char* tokens[BUFSIZE] = {0};
    int tokenNum = 0;
    myStrtok(path, tokens, &tokenNum);
    for(int i = 0 ; i < tokenNum; i++)
    {
        if(strcmp(tokens[i], ".") == 0)
        {
            continue;
        }
        else if(strcmp(tokens[i], "..") == 0)
        {
            if(pdir->length == 1)
            {
                return -1;
            }
            delDir(pdir);
        }
        else{
            addDir(pdir, tokens[i]);
        }

    }
    //printStack(pdir); // 打印当前栈中的内容，测试是否cd成功
    return 0;
}
//ls
int myLs(dirStack_t* pdir, char* dst)
{
    char path[BUFSIZE] = {0};
    getPath(pdir, path);
    DIR* dir = opendir(path);
    if(dir == NULL)
    {
        return -1;
    }

    //printStack(pdir);
    struct dirent* pdirent;
    errno = 0;
    while(1)
    {
        pdirent = readdir(dir);
        if(pdirent == NULL)
        {
            break;
        }
        char* name;
        name = pdirent->d_name;
        if(name[0] == '.')
        {
            continue;
        }
        strcat(dst, "\t");
        strcat(dst, name);
    }
    if(errno != 0)
    {
        return -1;
    }

    closedir(dir);
    return 0;
}
//pwd
//int myPwd(dirStack_t* pdir)
//{
//    char path[BUFSIZE] = {0};
//    getPath(pdir, path);
//    printf("pwd = %s\n", path);
//
//    return 0;
//}
//mkdir
int myMkdir(dirStack_t* pdir, char* pathName)
{
    char* tokens[BUFSIZE] = {0};
    int tokenNum = 0;
    myStrtok(pathName, tokens, &tokenNum);
    char presentWorkDir[BUFSIZE] = {0};
    getPath(pdir, presentWorkDir);

    for(int i = 0; i < tokenNum; i++)
    {
        char dirPath[BUFSIZE] = {0};
        snprintf(dirPath, sizeof(dirPath) + 1, "%s/%s", presentWorkDir, tokens[i]);
        int ret = mkdir(dirPath, 0777);
        if(ret == -1)
        {
            return -1;
        }
        memcpy(presentWorkDir, dirPath, sizeof(presentWorkDir));
    }

    return 0;
}
//rmdir
int deleteDir(const char* path)
{
    DIR* dir = opendir(path);
    if(dir == NULL)
    {
        return -1;
    }

    struct dirent* pdirent;
    while(1)
    {
        pdirent = readdir(dir);
        if(pdirent == NULL)
        {
            break;
        }
        char name[BUFSIZE] = {0};
        memcpy(name, pdirent->d_name, sizeof(name));
        if(name[0] == '.')
        {
            continue;
        }
        char subPath[BUFSIZE];
        snprintf(subPath, sizeof(subPath) + 1, "%s/%s", path, name);
        if(pdirent->d_type == DT_DIR)
        {
            deleteDir(subPath);
        }
        else
        {
            unlink(subPath);
        }
    }
    int ret = rmdir(path);
    if(ret == -1)
    {
        return -1;
    }

    closedir(dir);
    return 0;
}
int myRmdir(dirStack_t* pdir, char* pathName)
{
    char presentWorkDir[BUFSIZE] = {0};
    getPath(pdir, presentWorkDir);

    char dirPath[BUFSIZE] = {0};
    snprintf(dirPath, sizeof(dirPath) + 1, "%s/%s", presentWorkDir, pathName);
    deleteDir(dirPath);

    return 0;
}
//rm
int myRemove(dirStack_t* pdir, char* fileName)
{
    char presentWorkDir[BUFSIZE] = {0};
    getPath(pdir, presentWorkDir);

    char filePath[BUFSIZE] = {0};
    snprintf(filePath, sizeof(filePath) + 1, "%s/%s", presentWorkDir, fileName);
    if(unlink(filePath) == 0)
    {
        return 0;
    }
    else{
        return -1;
    }
}
