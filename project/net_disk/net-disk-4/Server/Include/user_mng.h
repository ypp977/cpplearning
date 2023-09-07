#ifndef _USER_MNG_
#define _USER_MNG_

#include <time.h>

#define K int
#define V int
#define TIME_INTVAL 60
#define MAP_SIZE 64

typedef struct u_hash_node_s{
    K key;
    V val;
    struct u_hash_node_s * next;
}u_hash_node_t;

typedef struct u_hash_map_s{
    u_hash_node_t hash_arr[MAP_SIZE];
    int size;
}u_map_t;

typedef struct u_set_s{
    int netfd;
    struct u_set_s *next;
    time_t time;
}u_set_t;

typedef struct u_cqueue_s{
    u_set_t queue[TIME_INTVAL];
    int size;
}u_cqueue_t;

// 创建循环队列（记得调用free_queue释放）
u_cqueue_t *init_queue();
// 释放queue
void free_queue(u_cqueue_t *cqueue);
// 创建map结构体（记得调用free_map释放）
u_map_t *init_map();
// 释放map
void free_map(u_map_t *map);
// 将用户加入队列
int add_user(int socket_fd, u_cqueue_t * u_cqueue, u_map_t *u_map);
// 踢人
int clear_user(int epfd, u_map_t *map, u_cqueue_t *cqueue);

#endif
