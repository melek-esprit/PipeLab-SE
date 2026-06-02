#ifndef SEM_UTILS_H
#define SEM_UTILS_H
#include <semaphore.h>
#include "common.h"

/* Synchronisation : sémaphores POSIX nommés */
sem_t *pl_sem_create(const char *name, unsigned int init);
sem_t *pl_sem_open  (const char *name);
void   pl_sem_close (sem_t *s);
void   pl_sem_unlink(const char *name);

#endif
