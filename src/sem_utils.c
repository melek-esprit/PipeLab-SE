#include "sem_utils.h"
#include <fcntl.h>
#include <sys/stat.h>

sem_t *pl_sem_create(const char *name, unsigned int init){
    sem_unlink(name);
    sem_t *s = sem_open(name, O_CREAT|O_EXCL, 0600, init);
    if(s == SEM_FAILED) die("sem_open create");
    return s;
}
sem_t *pl_sem_open(const char *name){
    sem_t *s = sem_open(name, 0);
    if(s == SEM_FAILED) die("sem_open");
    return s;
}
void pl_sem_close (sem_t *s){ sem_close(s); }
void pl_sem_unlink(const char *name){ sem_unlink(name); }
