#include "mq_utils.h"

mqd_t pl_mq_create(const char *name){
    mq_unlink(name);
    struct mq_attr a = {0};
    a.mq_maxmsg  = 10;
    a.mq_msgsize = sizeof(MqJob);
    mqd_t q = mq_open(name, O_CREAT|O_RDWR|O_EXCL, 0600, &a);
    if(q == (mqd_t)-1) die("mq_open create");
    return q;
}
mqd_t pl_mq_open(const char *name){
    mqd_t q = mq_open(name, O_RDWR);
    if(q == (mqd_t)-1) die("mq_open");
    return q;
}
void pl_mq_close (mqd_t q){ mq_close(q); }
void pl_mq_unlink(const char *name){ mq_unlink(name); }
