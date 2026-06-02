#include "thread_pool.h"

static void *worker_loop(void *arg){
    ThreadPool *tp = (ThreadPool*)arg;
    for(;;){
        pthread_mutex_lock(&tp->mtx);
        while(!tp->shutdown && tp->head == NULL)
            pthread_cond_wait(&tp->cv, &tp->mtx);
        if(tp->shutdown && tp->head == NULL){
            pthread_mutex_unlock(&tp->mtx);
            return NULL;
        }
        task_node *n = tp->head;
        tp->head = n->next;
        if(tp->head == NULL) tp->tail = NULL;
        pthread_mutex_unlock(&tp->mtx);

        n->fn(n->arg);
        free(n);

        pthread_mutex_lock(&tp->mtx);
        tp->pending--;
        if(tp->pending == 0) pthread_cond_broadcast(&tp->cv);
        pthread_mutex_unlock(&tp->mtx);
    }
}

ThreadPool *tp_create(int nb){
    ThreadPool *tp = calloc(1, sizeof(*tp));
    tp->nb_threads = nb;
    tp->workers    = calloc(nb, sizeof(pthread_t));
    pthread_mutex_init(&tp->mtx, NULL);
    pthread_cond_init (&tp->cv , NULL);
    for(int i=0;i<nb;i++)
        pthread_create(&tp->workers[i], NULL, worker_loop, tp);
    return tp;
}

void tp_submit(ThreadPool *tp, task_fn fn, void *arg){
    task_node *n = malloc(sizeof(*n));
    n->fn = fn; n->arg = arg; n->next = NULL;
    pthread_mutex_lock(&tp->mtx);
    if(tp->tail) tp->tail->next = n; else tp->head = n;
    tp->tail = n;
    tp->pending++;
    pthread_cond_signal(&tp->cv);
    pthread_mutex_unlock(&tp->mtx);
}

void tp_wait(ThreadPool *tp){
    pthread_mutex_lock(&tp->mtx);
    while(tp->pending > 0)
        pthread_cond_wait(&tp->cv, &tp->mtx);
    pthread_mutex_unlock(&tp->mtx);
}

void tp_destroy(ThreadPool *tp){
    pthread_mutex_lock(&tp->mtx);
    tp->shutdown = 1;
    pthread_cond_broadcast(&tp->cv);
    pthread_mutex_unlock(&tp->mtx);
    for(int i=0;i<tp->nb_threads;i++) pthread_join(tp->workers[i], NULL);
    pthread_mutex_destroy(&tp->mtx);
    pthread_cond_destroy(&tp->cv);
    free(tp->workers); free(tp);
}
