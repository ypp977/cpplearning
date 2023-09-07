#include <func.h>
#include "../Include/user_mng.h"
#include "../Include/msg.h"
#include "../Include/head.h"

static u_hash_node_t *init_h_node( K key, V val){
    u_hash_node_t *node = (u_hash_node_t *)malloc(sizeof(u_hash_node_t));
    if(!node)
        return NULL;
    node->key = key;
    node->val = val;
    node->next = NULL;
    return node;
}

static void free_h_node(u_hash_node_t *node){
    free(node);
}

static int init_map_helper(u_map_t *map){
    if(!map)
        return -1;
    map->size = MAP_SIZE;
    for(int i = 0; i < map->size; ++i){
        map->hash_arr[i].key = 0;
        map->hash_arr[i].val = 0;
        map->hash_arr[i].next = NULL;
    }
    return 0;
}

static u_hash_node_t *find_map_node(u_map_t *map, K key){
    if(!map){
        return NULL;
    }
    int index = key % MAP_SIZE;
    u_hash_node_t *p = &map->hash_arr[index];
    while(p->next && p->next->key != key){
        p = p->next;
    }
    if(!p->next)
        return NULL;
    return p->next;
}

static int add_map_node(u_map_t *map, K key, V val){
    u_hash_node_t *node = init_h_node(key,val);
    if(!map || !node)
        return -1;
    int index = node->key % MAP_SIZE;
    u_hash_node_t * p = &map->hash_arr[index];
    while(p && p->next){
        p=p->next;    
    }
    p->next = node;
    printf("%d add_hash\n", node->key);
    return 0;
}

static int del_map_node(u_map_t *map, K key){
    if(!map)
        return -1;
    int index = key % MAP_SIZE;
    u_hash_node_t * p = &map->hash_arr[index];
    while(p->next && p->next->key != key){
        p = p->next;
    }
    if(!p->next)
        return -1;
    u_hash_node_t *del = p->next;
    p->next = del->next;
    printf("%d del_hash\n", del->key);
    free_h_node(del);
    return 0;
}

static int update_map_node(u_map_t *map, K key, V val){
    if(!map)
        return -1;
    int index = key % MAP_SIZE;
    u_hash_node_t * p = map->hash_arr[index].next;
    while(p && p->key != key){
        p = p->next;
    }
    if(!p)
        return -1;
    p->val = val;
    printf("%d update_hash\n", p->key);
    return 0;
}

static u_set_t *init_set_node( int netfd, time_t time){
    u_set_t *node = (u_set_t *)malloc(sizeof(u_set_t));
    if(!node)
        return NULL;
    node->netfd = netfd;
    node->time = time;
    node->next = NULL;
    return node;
}

static void free_set_node(u_set_t *node){
    free(node);
}

static int init_u_cqueue(u_cqueue_t *cqueue){
    if(!cqueue)
        return -1;
    cqueue->size = TIME_INTVAL;
    for(int i = 0; i < cqueue->size; ++i){
        cqueue->queue[i].netfd = 0;
        cqueue->queue[i].time = 0;
        cqueue->queue[i].next = 0;
    }
    return 0;
}

static int add_u_set_node(u_cqueue_t *cqueue, int index, int netfd, time_t time){
    if(!cqueue)
        return -1;
    u_set_t *toAdd = init_set_node(netfd, time);
    u_set_t *p = &cqueue->queue[index];
    while(p && p->next){
        p = p->next;
    }
    p->next = toAdd;
    printf("%d Add\n", toAdd->netfd);
    return 0;
}

static int del_u_set_node(u_cqueue_t * cqueue,int index, int netfd){
    if(!cqueue)
        return -1;
    u_set_t *p = &cqueue->queue[index];
    if(p->next && p->next->netfd != netfd){
        p = p->next;
    }
    if(p->next == NULL)
        return -1;
    u_set_t *toDel = p->next;
    p->next = toDel->next;
    printf("%d del\n", toDel->netfd);
    free(toDel);
    return 0;
}

static u_set_t *find_u_set_node(u_cqueue_t *cqueue, int index, int netfd){
    if(!cqueue)
        return NULL;
    u_set_t *p = &cqueue->queue[index];
    while(p->next && p->next->netfd != netfd){
        p = p->next;
    }
    if(p->next)
        return p->next;
    return NULL;
}

u_cqueue_t *init_queue(){
    u_cqueue_t * rslt = (u_cqueue_t *)malloc(sizeof(u_cqueue_t));
    init_u_cqueue(rslt);
    return rslt;
}

void free_queue(u_cqueue_t *cqueue){
    free(cqueue);
}

u_map_t *init_map(){
    u_map_t * rslt = (u_map_t *)malloc(sizeof(u_map_t));
    init_map_helper(rslt);
    return rslt;
}

void free_map(u_map_t *map){
    free(map);
}

int add_user(int socket_fd, u_cqueue_t * u_cqueue, u_map_t *u_map){
    u_hash_node_t *p = find_map_node(u_map, socket_fd);
    int ret = 0;
    if(p){
        int q_index = p->val;
        ret = del_u_set_node(u_cqueue, q_index, socket_fd);
        if(ret)
            return ret;
        time_t cur_time = time(NULL);
        q_index = cur_time  % TIME_INTVAL;
        ret = add_u_set_node(u_cqueue, q_index, socket_fd, cur_time);
        if(ret)
            return ret;
        ret = update_map_node(u_map, socket_fd, q_index);
        if(ret)
            return ret;
    }else{
        int q_index = 0;
        time_t cur_time = time(NULL);
        q_index = cur_time % TIME_INTVAL;
        ret = add_u_set_node(u_cqueue, q_index, socket_fd, cur_time);
        if(ret)
            return ret;
        ret = add_map_node(u_map, socket_fd, q_index);
        if(ret)
            return ret;
    }
    return 0;
}

int clear_user(int epfd, u_map_t *map, u_cqueue_t *cqueue){
    time_t cur_time = time(NULL);
    int q_index = cur_time % TIME_INTVAL;
    int count = 0;
    if(cqueue->queue[q_index].next == NULL)
        return 0;
    u_set_t *p = &cqueue->queue[q_index];
    while(p->next){
        u_set_t *toDel = p->next;
        if(toDel->time == cur_time){
            p = p->next;
            continue;
        }
        int ret = 0;
        ret = del_map_node(map, toDel->netfd);
        if(ret){
            return ret;
        }
        struct msg_t msg;
        epollDel(epfd, toDel->netfd);        
        close(toDel->netfd);
        p->next = toDel->next;
        printf("%d del\n", toDel->netfd);
        free_set_node(toDel);
        ++count;
    }
    return count;
}

/* int main(){ */
/*     u_cqueue_t *cqueue = init_queue(); */
/*     u_map_t * map = init_map(); */

/*     for(int i = 1; i <= 10; ++i){ */
/*         add_user(i, cqueue, map); */
/*     } */
/*     for(int i = 1; i <= 10; ++i){ */
/*         sleep(1); */
/*         printf("%d:\n", i); */
/*         add_user(i, cqueue, map); */
/*     } */
/*     for(int i = 1; i <= 10; ++i){ */
/*         sleep(1); */
/*         printf("%d:\n", i); */
/*         add_user(i+10, cqueue, map); */
/*     } */
/*     for(int i = 1; i <= 10; ++i){ */
/*         sleep(1); */
/*         printf("%d:\n", i); */
/*         int count = clear_user(map, cqueue); */
/*         printf("%d\n", count); */
/*     } */
/*     free_map(map); */
/*     free_queue(cqueue); */
/*     return 0; */
/* } */
