#include "taskQueue.h"
int initQueue(taskQueue_t *pqueue){
    bzero(pqueue,sizeof(taskQueue_t));
    pthread_mutex_init(&pqueue->mutex,NULL);
    pthread_cond_init(&pqueue->cond,NULL);
    return 0;
}
int enQueue(taskQueue_t *pqueue, int netfd){
    task_t * pNew = (task_t *)malloc(sizeof(task_t));
    pNew->netfd = netfd;
    pNew->pNext = NULL;

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

