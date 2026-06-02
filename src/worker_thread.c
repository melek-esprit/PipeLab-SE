/*
 * Module : Multithreads
 * Couvre :
 *   - pthread_create / pthread_join (via thread_pool)
 *   - mémoire partagée pour cumul des stats
 *   - sémaphore pour exclusion mutuelle
 */
#include "common.h"
#include "thread_pool.h"
#include "shm_utils.h"
#include "sem_utils.h"
#include <semaphore.h>

typedef struct {
    char  filename[MAX_PATH];
    long  start_byte;
    long  end_byte;
    SharedStats *stats;
    sem_t *mutex;
} ThreadJob;

static void process_chunk(void *arg){
    ThreadJob *j = (ThreadJob*)arg;
    FILE *f = fopen(j->filename, "r");
    if(!f){ free(j); return; }
    fseek(f, j->start_byte, SEEK_SET);

    long lines=0, words=0, bytes=0;
    int in_word = 0, c;
    long pos = j->start_byte;
    while(pos < j->end_byte && (c = fgetc(f)) != EOF){
        bytes++; pos++;
        if(c == '\n') lines++;
        if(c==' '||c=='\t'||c=='\n'){
            if(in_word){ words++; in_word = 0; }
        } else in_word = 1;
    }
    if(in_word) words++;
    fclose(f);

    /* Section critique : sémaphore POSIX (mutex binaire) */
    sem_wait(j->mutex);
    j->stats->total_lines += lines;
    j->stats->total_words += words;
    j->stats->total_bytes += bytes;
    j->stats->jobs_done   += 1;
    sem_post(j->mutex);

    free(j);
}

void run_multithread(const Config *cfg, SharedStats *stats, sem_t *mutex){
    ThreadPool *tp = tp_create(cfg->nb_threads);

    FILE *f = fopen(cfg->input_file, "r");
    if(!f) die("open input");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);

    int chunks = cfg->nb_threads * 2;              /* sur-découpage */
    long step  = size / chunks;
    for(int i=0; i<chunks; i++){
        ThreadJob *j = malloc(sizeof(*j));
        strcpy(j->filename, cfg->input_file);
        j->start_byte = i*step;
        j->end_byte   = (i==chunks-1) ? size : (i+1)*step;
        j->stats = stats; j->mutex = mutex;
        tp_submit(tp, process_chunk, j);
    }
    tp_wait(tp);
    tp_destroy(tp);
}
