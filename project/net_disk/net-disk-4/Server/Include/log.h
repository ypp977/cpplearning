#ifndef __LOG__
#define __LOG__

#include <mysql/mysql.h>
#include <func.h>

#define addLog(info, type, mysql) {                                                            \
    printf("file:%s  function:%s  line:%d  info:%s\n", __FILE__, __func__, __LINE__, info); \
    char sql[1024] = {0};                                                                   \
    time_t now = time(NULL);                                                                \
    char *pTime = ctime(&now);                                                              \
    pTime[strcspn(pTime, "\n")] = '\0';                                                     \
    sprintf(sql, "insert into log (file, function_name, line, time, info, type) values(\'%s\', \'%s\', %d, \'%s\', \'%s\', %d);",__FILE__, __func__, __LINE__, pTime, info, type);\
    int qret = mysql_query(mysql, sql);                                                     \
    if(qret != 0){\
        fprintf(stderr, "mysql_query: %s\n", mysql_error(mysql));                           \
    }\
}
#endif