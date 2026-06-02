#ifndef MQ_UTILS_H
#define MQ_UTILS_H
#include <mqueue.h>
#include "common.h"

/* IPC #3 : Files de messages POSIX */
typedef struct {
    int    job_id;
    long   start_line;
    long   end_line;
    char   payload[MAX_LINE];
} MqJob;

mqd_t pl_mq_create(const char *name);
mqd_t pl_mq_open  (const char *name);
void  pl_mq_close (mqd_t q);
void  pl_mq_unlink(const char *name);

#endif
