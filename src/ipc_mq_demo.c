/*
 * Module : IPC par files de messages POSIX (mq_open/mq_send/mq_receive).
 * Pattern producteur/consommateur multi-processus.
 */
#include "common.h"
#include "mq_utils.h"

void run_mq_demo(const Config *cfg){
    pl_mq_unlink(MQ_NAME);
    mqd_t q = pl_mq_create(MQ_NAME);
    mq_close(q);

    int N = cfg->nb_processes;

    /* Consommateurs */
    for(int i=0;i<N;i++){
        if(fork() == 0){
            mqd_t qc = pl_mq_open(MQ_NAME);
            MqJob job;
            for(;;){
                if(mq_receive(qc, (char*)&job, sizeof(job), NULL) < 0) break;
                if(job.job_id < 0) break;            /* poison pill */
                printf("[Conso %d] job=%d range=[%ld..%ld]\n",
                       i, job.job_id, job.start_line, job.end_line);
            }
            mq_close(qc);
            exit(0);
        }
    }

    /* Producteur (père) */
    mqd_t qp = pl_mq_open(MQ_NAME);
    int total_jobs = N * 3;
    for(int j=0; j<total_jobs; j++){
        MqJob job = {0};
        job.job_id     = j;
        job.start_line = j*cfg->chunk_size;
        job.end_line   = (j+1)*cfg->chunk_size;
        snprintf(job.payload, sizeof(job.payload),
                 "chunk %d de %s", j, cfg->input_file);
        mq_send(qp, (const char*)&job, sizeof(job), 0);
    }
    /* poison pills */
    for(int i=0;i<N;i++){
        MqJob end = {0}; end.job_id = -1;
        mq_send(qp, (const char*)&end, sizeof(end), 0);
    }
    mq_close(qp);

    while(wait(NULL) > 0);
    pl_mq_unlink(MQ_NAME);
}
