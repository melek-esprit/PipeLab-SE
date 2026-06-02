/*
 * Module : Problème classique du Barbier Endormi (Dijkstra)
 * Synchronisation : 3 sémaphores + mutex.
 */
#include "common.h"
#include <pthread.h>
#include <semaphore.h>

static sem_t  customers;     /* clients en attente   */
static sem_t  barber_sem;    /* barbier prêt         */
static sem_t  mutex;         /* protection waiting   */
static int    waiting   = 0;
static int    CHAIRS    = 3;
static int    served    = 0;
static int    rejected  = 0;
static int    stop_flag = 0;

static void *barber_thread(void *_){
    (void)_;
    while(!stop_flag){
        sem_wait(&customers);
        if(stop_flag) break;
        sem_wait(&mutex);
          waiting--;
          sem_post(&barber_sem);
        sem_post(&mutex);
        printf("[Barbier] coupe en cours...\n"); usleep(60000);
        served++;
    }
    return NULL;
}

typedef struct { int id; } CustArg;

static void *customer(void *a){
    CustArg *c = a;
    sem_wait(&mutex);
    if(waiting < CHAIRS){
        waiting++;
        printf("[Client %d] s'assied (waiting=%d)\n", c->id, waiting);
        sem_post(&customers);
        sem_post(&mutex);
        sem_wait(&barber_sem);
        printf("[Client %d] coiffé\n", c->id);
    } else {
        rejected++;
        printf("[Client %d] salon plein, repart\n", c->id);
        sem_post(&mutex);
    }
    return NULL;
}

void run_barber(int chairs, int nb_customers){
    CHAIRS = chairs;
    sem_init(&customers,  0, 0);
    sem_init(&barber_sem, 0, 0);
    sem_init(&mutex,      0, 1);

    pthread_t bt; pthread_create(&bt, NULL, barber_thread, NULL);

    pthread_t *ct = calloc(nb_customers, sizeof(pthread_t));
    CustArg   *ca = calloc(nb_customers, sizeof(CustArg));
    for(int i=0;i<nb_customers;i++){
        ca[i].id = i;
        pthread_create(&ct[i], NULL, customer, &ca[i]);
        usleep(20000);
    }
    for(int i=0;i<nb_customers;i++) pthread_join(ct[i], NULL);

    /* arrêt propre */
    stop_flag = 1;
    sem_post(&customers);
    pthread_join(bt, NULL);

    sem_destroy(&customers); sem_destroy(&barber_sem); sem_destroy(&mutex);
    free(ct); free(ca);
    printf("\n[Barbier] servis=%d  refusés=%d\n", served, rejected);
}
