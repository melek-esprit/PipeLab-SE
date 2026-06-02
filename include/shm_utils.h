#ifndef SHM_UTILS_H
#define SHM_UTILS_H
#include "common.h"
#include <sys/mman.h>

/* IPC #2 : Mémoire partagée POSIX (shm_open + mmap) */
typedef struct {
    long total_lines;
    long total_words;
    long total_bytes;
    long jobs_done;
} SharedStats;

SharedStats *shm_create(const char *name);
SharedStats *shm_open_existing(const char *name);
void         shm_destroy(const char *name, SharedStats *p);

#endif
