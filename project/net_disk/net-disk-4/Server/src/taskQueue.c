#include "../Include/taskQueue.h"

int initQueue(taskQueue_t *pqueue){
    bzero(pqueue,sizeof(taskQueue_t));
    pthread_mutex_init(&pqueue->mutex,NULL);
    pthread_cond_init(&pqueue->cond,NULL);
   bzero(pqueue->netfd2Usr, 4096);
    return 0;
}
int enQueue(taskQueue_t *pqueue, int netfd,int flag, char* path){
    task_t * pNew = (task_t *)malloc(sizeof(task_t));
    pNew->netfd = netfd;
    pNew->pNext = NULL;
    pNew->flag = flag;
    strncpy(pNew->buf, path, BUFSIZE);
    printf("enQueue:create!\n");

    if(pqueue->queueSize == 0){
        pqueue->pFront = pNew;
        pqueue->pRear = pNew;
    }
    else{
        pqueue->pRear->pNext = pNew;
        pqueue->pRear = pNew;
    }
    ++pqueue->queueSize;

    return 0;
}
int deQueue(taskQueue_t *pqueue){
    task_t * pCur = pqueue->pFront;
    pqueue->pFront = pCur->pNext;
    free(pCur);
    if(pqueue->queueSize == 1){
        pqueue->pRear = NULL;
    }
    --pqueue->queueSize;
    return 0;
}


