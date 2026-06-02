/*
 * Module : Problème classique des Philosophes Dîneurs
 * Synchronisation par sémaphores POSIX (un sémaphore par fourchette).
 * Solution Dijkstra : numérotation asymétrique pour éviter l'interblocage.
 */
#include "common.h"
#include <pthread.h>
#include <semaphore.h>

#define MAX_PHIL 32
static sem_t  forks[MAX_PHIL];
static int    N_PHIL = 5;
static int    eats[MAX_PHIL];

typedef struct { int id; int rounds; } PhilArg;

static void think(int id){ printf("[Phil %d] pense\n", id); usleep(50000); }
static void eat  (int id){ printf("[Phil %d] mange\n", id); usleep(80000); }

static void *philosopher(void *a){
    PhilArg *p = a;
    int id = p->id, left = id, right = (id+1) % N_PHIL;
    for(int r=0; r<p->rounds; r++){
        think(id);
        /* Dijkstra : les pairs prennent gauche puis droite,
                      les impairs droite puis gauche                       */
        if(id % 2 == 0){ sem_wait(&forks[left]);  sem_wait(&forks[right]); }
        else           { sem_wait(&forks[right]); sem_wait(&forks[left ]); }

        eat(id); eats[id]++;

        sem_post(&forks[left]); sem_post(&forks[right]);
    }
    return NULL;
}

void run_philosophers(int n, int rounds){
    if(n > MAX_PHIL) n = MAX_PHIL;
    N_PHIL = n;
    for(int i=0;i<n;i++){ sem_init(&forks[i],0,1); eats[i]=0; }

    pthread_t  th[MAX_PHIL];
    PhilArg    args[MAX_PHIL];
    for(int i=0;i<n;i++){
        args[i].id = i; args[i].rounds = rounds;
        pthread_create(&th[i], NULL, philosopher, &args[i]);
    }
    for(int i=0;i<n;i++) pthread_join(th[i], NULL);
    for(int i=0;i<n;i++) sem_destroy(&forks[i]);

    printf("\n[Philosophes] résultat : ");
    for(int i=0;i<n;i++) printf("P%d=%d  ", i, eats[i]);
    printf("\n");
}
