#ifndef COMMON_H
#define COMMON_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_PROC      64
#define MAX_THREADS   128
#define MAX_LINE      4096
#define MAX_PATH      512
#define SHM_NAME      "/pipelab_shm"
#define SEM_MUTEX     "/pipelab_sem_mutex"
#define MQ_NAME       "/pipelab_mq"

typedef struct {
    int    nb_processes;     /* configurable */
    int    nb_threads;       /* configurable */
    char   input_file[MAX_PATH];
    char   command[64];      /* wc, grep, sort ... (recouvrement par exec) */
    char   pattern[128];     /* pour grep */
    int    chunk_size;       /* lignes par tâche */
    int    nb_philosophers;  /* problème classique */
    int    nb_barber_chairs; /* problème classique */
} Config;

double now_ms(void);
int load_config(const char *path, Config *cfg);
void die(const char *msg);

#endif
