#ifndef __LOG__
#define __LOG__

#include<func.h>
#include<syslog.h>
#include<time.h>

#define NETDISK_LOG_INFO(info) {  \
    char logInfo[4096];\
    time_t now = time(NULL);\
    char *timeStr = ctime(&now);\
    timeStr[strcspn(timeStr, "\n")] = ' ';\
    sprintf(logInfo, "time:%s   type:%s   message:%s", timeStr, "normal", (char*)info); \
    syslog(LOG_INFO,"%s", logInfo);\
}

#define NETDISK_LOG_ERR(info) {  \
    char logInfo[4096];\
    time_t now = time(NULL);\
    char *timeStr = ctime(&now);\
    timeStr[strcspn(timeStr, "\n")] = ' ';\
    sprintf(logInfo, "time:%s   type:%s    file:%s   function:%s   line:%d   message:%s", timeStr,"error", __FILE__, __FUNCTION__, __LINE__, (char*)info); \
    syslog(LOG_ERR,"%s", logInfo);\
}

#endif
