#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/epoll.h>
#include "./tcp_init.h"
#include "./client_work.h"
#include "./msg.h"
#include "./md5.h"
#include "./getCryptPasswd.h"
#include "./c_file.h"
#define ARGS_CHECK(argc, n) {               \
    if (argc != n) {                        \
        fprintf(stderr, "Error: expected %d arguments\n", n);   \
        exit(1);                            \
    }                                       \
}

#define ERROR_CHECK(retval, val, msg) {    \
    if (retval == val) {                   \
        perror(msg);                       \
        exit(1);                           \
    }                                      \
}






