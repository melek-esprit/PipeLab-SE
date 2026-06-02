#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <pthread.h>
#include "common.h"

typedef void (*task_fn)(void *arg);

typedef struct task_node {
    task_fn fn;
    void   *arg;
    struct task_node *next;
} task_node;

typedef struct {
    pthread_t       *workers;
    int              nb_threads;
    task_node       *head, *tail;
    pthread_mutex_t  mtx;
    pthread_cond_t   cv;
    int              shutdown;
    int              pending;
} ThreadPool;

ThreadPool *tp_create(int nb_threads);
void        tp_submit(ThreadPool *tp, task_fn fn, void *arg);
void        tp_wait  (ThreadPool *tp);
void        tp_destroy(ThreadPool *tp);

#endif
