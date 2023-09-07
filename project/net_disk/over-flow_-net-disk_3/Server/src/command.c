#include "../Include/dir.h"
// 调试函数-打印当前栈的内容
int printStack(dirStack_t* pdir) {
    dirNode_t* p = pdir->top;
    while (p) {
        printf("curDir = %s\n", p->dirName);
        p = p->next;
    }
    return 0;
}

// 分词
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
//INSET INTO                                                                                                                                                           
int insertInto(MYSQL* mysql, const char* filename, const char* type, 
               int pre_id, const char* username, const char* path)
{
    //  printf("filename = %s\n", filename);
    //  printf("pre_id = %d\n", pre_id);
    //  printf("type = %s\n", type);
    //  printf("username = %s\n", username);
    //insert to
    char sql[BUFSIZE] = {0};
    snprintf(sql,sizeof(sql), "INSERT INTO fileInfo values(NULL, '%s', '%s', '%d', '%s', '%s', 0, NULL);"
             , filename, type, pre_id, username, path);
    int qret = mysql_query(mysql, sql);//执行
    if(qret != 0)//报错
    {
        fprintf(stderr, "insertInto insert -> mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
   return 0;
}

//初始化
int initCwd(userCwd_t* pUcwd,const char* username)
{
    //初始化用户名；
    strncpy(pUcwd->user, username, sizeof(pUcwd->user)); 
    //初始化cwd
    pUcwd->cwd = -1;
    pUcwd->dirstack.length = 0;
    pUcwd->dirstack.top = NULL;

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
//提取路径
int getPath(const dirStack_t* pdir, char* pathName)
{
    bzero(pathName, sizeof(pathName));
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
    sprintf(pathName, "/%s" ,arr[0].dirName);
    for(int i = 1; i < n ; i++)
    {
        sprintf(pathName, "%s/%s", pathName, arr[i].dirName);
    }

    printf("curStack = %s\n", pathName);
    return 0;
}
int path2Id(const char* username, MYSQL* mysql, const char* path)
{
    char sql[BUFSIZE] = {0};
    //获取当前目录下文件名
    sprintf(sql, "SELECT id FROM fileInfo WHERE username = '%s' AND path = '%s' AND tomb = 0;",
            username, path);
    printf("path2Id sql = %s\n", sql);
    int qret = mysql_query(mysql, sql);
    if(qret != 0)
    {
        fprintf(stderr, "path2Id select -> mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
    MYSQL_RES* res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    if(row == 0)
    {
        printf("%s do not exist!\n", path);
        return -1;
    }
    // 每次select之后，必须释放结果集，不然会影响下次select的执行
    // mysql query:Commands out of sync; you can't run this command now
    // 释放结果集
    mysql_free_result(res);
    return atoi(row[0]); 
}
int dirIsExist(userCwd_t* tmpUsr, MYSQL* mysql, const char* path)
{
    //分词
    char *tokens[BUFSIZE] = {0};
    int tokenNum = 0;
    char dstPath[BUFSIZE] = {0};
    strncpy(dstPath, path, sizeof(dstPath));
    myStrtok(dstPath, tokens, &tokenNum);
    //遍历
    for(int i = 0; i < tokenNum; i++)
    {
        //.
        if(strcmp(tokens[i], ".") == 0)
        {
            continue;
        }
        else if(strcmp(tokens[i], "..") == 0)
        {
            if(tmpUsr->dirstack.length == 0)
            {
                continue;
            }
            delDir(&tmpUsr->dirstack);
        }
        else
        {
            addDir(&tmpUsr->dirstack, tokens[i]);    
        }
    }
    //如果tmpstack->length为0
    if(tmpUsr->dirstack.length == 0)
    {
        //在家目录
        tmpUsr->cwd = -1;
        return 0;//根目录不给删除       
    }
    char newPath[BUFSIZE/2 + 1] = {0};
    getPath(&tmpUsr->dirstack, newPath);
    char sql[BUFSIZE] = {0};
    //获取id和type
    snprintf(sql, sizeof(sql),"SELECT id,type FROM fileInfo WHERE username = '%s' AND path = '%s' AND tomb = 0;",
            tmpUsr->user, newPath);
    printf("sql = %s\n", sql);
    int qret = mysql_query(mysql, sql);
    if(qret != 0)
    {
        fprintf(stderr, "dirIsExist select -> mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
    MYSQL_RES* res = mysql_store_result(mysql);
    MYSQL_ROW row = mysql_fetch_row(res);
    if(row == NULL)
    {
        //既没有文件也没有目录
        printf("%s do not exist!\n", newPath);
        return -1;
    }
    tmpUsr->cwd = atoi(row[0]);
    if(strcmp(row[1], "f") == 0)
    {
        //是文件
        return 1;
    }

    //是目录
    // 释放结果集
    mysql_free_result(res);
    
    return 0;
}
//cd
int myCd(userCwd_t* pUcwd, MYSQL* mysql,const char* dst)
{
    //创建一个临时结构体
    userCwd_t tmpUsr;
    memcpy(&tmpUsr, pUcwd, sizeof(tmpUsr));
    int ret = dirIsExist(&tmpUsr, mysql, dst);
    if(ret == 0)
    {
    pUcwd->cwd = tmpUsr.cwd;
    memcpy(&pUcwd->dirstack, &tmpUsr.dirstack, sizeof(pUcwd->dirstack));
    }
    else{
        return -1;
    }
    printStack(&pUcwd->dirstack);

    return 0;
}
//ls
int myLs(const userCwd_t* pUcwd, MYSQL* mysql, char* dst)
{
    bzero(dst, BUFSIZE);
    char sql[BUFSIZE] = {0};
    //获取当前目录下文件名
    sprintf(sql, "SELECT filename FROM fileInfo WHERE pre_id = '%d' AND tomb = '0';", pUcwd->cwd);
    printf("sql = %s\n", sql);
    int qret = mysql_query(mysql, sql);
    if(qret != 0)
    {
        fprintf(stderr, "myLs select -> mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
    MYSQL_RES* res = mysql_store_result(mysql);
    int cnt = 0;//文件数
    for(unsigned i = 0; i < mysql_num_rows(res); i++)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        strncat(dst, "\t", BUFSIZE);
        strncat(dst, row[0], BUFSIZE);
        cnt++;
        if(cnt % 6 == 0 )//一行的文件数等于6换行
        {
            strncat(dst, "\n", BUFSIZE);
        }
    }
    if(cnt % 6 != 0)
    {
        strncat(dst, "\n", sizeof(dst));
    }
    mysql_free_result(res);
    return 0;
}
//pwd
int myPwd(const userCwd_t* pUcwd, char* pwd)
{
    char path[BUFSIZE + 1] = {0};
    getPath(&pUcwd->dirstack, path);
    snprintf(pwd, BUFSIZE + 255, "%s%s", pUcwd->user, path);
    return 0;
}
//mkdir
int myMkdir(const userCwd_t* pUcwd, MYSQL* mysql,const char* pathName)
{
    printf("mkdir:\n");
    printf("pathName = %s\n",pathName);
    //创建一个临时结构体
    dirStack_t tmpstack;
    memcpy(&tmpstack, &pUcwd->dirstack, sizeof(tmpstack));
    //分词
    char *tokens[BUFSIZE] = {0};
    int tokenNum = 0;
    char dstPath[BUFSIZE] = {0};
    strncpy(dstPath, pathName, sizeof(dstPath));
    myStrtok(dstPath, tokens, &tokenNum);
    //遍历
    for(int i = 0; i < tokenNum; i++)
    {
        if(strcmp(tokens[i], ".") == 0)
        {
            continue;
        }
        else if(strcmp(tokens[i], "..") == 0)
        {
            if(tmpstack.length == 0)
            {
                continue;
            }
            delDir(&tmpstack);
        }
        else
        {
            char idPath[BUFSIZE/2 + 1] = {0};
            getPath(&tmpstack, idPath);
            int pre_id = 0;
            if(tmpstack.length == 0)
            {
                pre_id = -1;
            }
            else{
                pre_id = path2Id(pUcwd->user, mysql, idPath);
            }
            addDir(&tmpstack, tokens[i]);    

            getPath(&tmpstack, idPath);
            //判断tomb是否为0
            char sql[BUFSIZE] = {0};
            sprintf(sql, "SELECT tomb FROM fileInfo WHERE username = '%s' AND path = '%s';",
                    pUcwd->user, idPath);
            int qret = mysql_query(mysql, sql);
            if(qret != 0)
            {
                fprintf(stderr, "myMkdir select -> mysql_query:%s\n", mysql_error(mysql));
                return -1;
            }
            MYSQL_RES* res = mysql_store_result(mysql);
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row == NULL)
            {
                // 先释放结果集
                mysql_free_result(res);
                //不存在,直接插入
                insertInto(mysql, tokens[i], "d", pre_id, pUcwd->user, idPath);
            }
            else if(strcmp(row[0], "1") == 0)
            {
                //存在但死了，复活
                mysql_free_result(res);//先释放
                //UPDATE把tmpUcwd的tomb置为0
                sprintf(sql, "UPDATE fileInfo SET tomb = 0 WHERE username = '%s' AND path = '%s';",
                        pUcwd->user, idPath);
                printf("sql = %s\n", sql);
                int qret = mysql_query(mysql, sql);
                if(qret != 0)
                {
                    fprintf(stderr, "myMkdir update -> mysql_query:%s\n", mysql_error(mysql));
                    return -1;
                }
            }
        }
    }

    return 0;
}
int myRemove(int id, MYSQL* mysql)
{
    //UPDATE把tmpUcwd的tomb置为1
    char sql[BUFSIZE] = {0};
    sprintf(sql, "UPDATE fileInfo SET tomb = 1 WHERE id = %d;", id);
    int qret = mysql_query(mysql, sql);
    if(qret != 0)
    {
        fprintf(stderr, "myRemove update -> mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
    return 0;
}   
//rmdir
int deleteDir(int id,const char* username, MYSQL* mysql)
{
    //获取tmp目录下的所有目录和文件的id和类型
    char sql[BUFSIZE] = {0};
    sprintf(sql, "SELECT id,type FROM fileInfo WHERE pre_id = %d AND username = '%s' AND tomb = '0';"
            , id, username);
    //执行
    int qret = mysql_query(mysql, sql);
    if(qret != 0)
    {
        fprintf(stderr, "deleteDir select -> mysql_query:%s\n", mysql_error(mysql));
        return -1;
    }
    //读返回值
    MYSQL_RES* res = mysql_store_result(mysql);
    for(unsigned i = 0; i < mysql_num_rows(res); i++)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        //目录递归删除
        if(strcmp(row[1], "d") == 0)
        {
            deleteDir(atoi(row[0]), username, mysql);
        }
        else
        {
            myRemove(atoi(row[0]), mysql);
        }
    }
    mysql_free_result(res);
    myRemove(id, mysql);

    return 0;
}
//不能删除文件
int myRmdir(userCwd_t* pUcwd, MYSQL* mysql,const char* pathName)
{
    //创建一个临时结构体
    userCwd_t tmpUsr;
    memcpy(&tmpUsr, pUcwd, sizeof(tmpUsr));
    int ret = dirIsExist(&tmpUsr, mysql, pathName);
    if(ret == -1)
    {
        printf("%s illegal!\n", pathName);
        return -1;
    }
        deleteDir(tmpUsr.cwd, tmpUsr.user, mysql); 
    return 0;
}
