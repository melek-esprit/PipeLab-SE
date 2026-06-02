/*
 * PipeLab++ — Orchestrateur principal
 *
 * Modes :
 *   ./pipelab proc        : multiprocessus + pipe + exec  (recouvrement)
 *   ./pipelab thread      : multithreads + shm + sémaphore
 *   ./pipelab philo       : philosophes dîneurs
 *   ./pipelab barber      : barbier endormi
 *   ./pipelab mq          : files de messages
 *   ./pipelab bench       : benchmark complet (compare proc vs thread vs séq)
 *   ./pipelab all         : tous les modes successivement
 *
 * Configuration : config/config.ini  (modifiable sans recompilation)
 */
#include "common.h"
#include "shm_utils.h"
#include "sem_utils.h"
#include "bench.h"

extern long run_multiprocess(const Config *cfg);
extern void run_multithread (const Config *cfg, SharedStats *stats, sem_t *m);
extern void run_philosophers(int n, int rounds);
extern void run_barber      (int chairs, int nb_customers);
extern void run_mq_demo     (const Config *cfg);

static long run_sequential(const Config *cfg){
    FILE *f = fopen(cfg->input_file, "r");
    if(!f) die("open input");
    long lines = 0; int c;
    while((c = fgetc(f)) != EOF) if(c=='\n') lines++;
    fclose(f);
    return lines;
}

static void usage(const char *p){
    printf("Usage: %s <proc|thread|philo|barber|mq|bench|all> [config]\n", p);
}

int main(int argc, char **argv){
    if(argc < 2){ usage(argv[0]); return 1; }
    const char *mode = argv[1];
    const char *cfgp = (argc >= 3) ? argv[2] : "config/config.ini";

    Config cfg;
    load_config(cfgp, &cfg);
    printf("==> Config: procs=%d threads=%d cmd=%s file=%s philos=%d chairs=%d\n",
           cfg.nb_processes, cfg.nb_threads, cfg.command,
           cfg.input_file, cfg.nb_philosophers, cfg.nb_barber_chairs);

    if(!strcmp(mode, "proc")){
        double t = bench_start();
        long l = run_multiprocess(&cfg);
        printf("Total: %ld  (%.2f ms)\n", l, bench_stop(t));
    }
    else if(!strcmp(mode, "thread")){
        sem_t *m = pl_sem_create(SEM_MUTEX, 1);
        SharedStats *s = shm_create(SHM_NAME);
        double t = bench_start();
        run_multithread(&cfg, s, m);
        double dt = bench_stop(t);
        printf("Lignes=%ld  Mots=%ld  Octets=%ld  Jobs=%ld  (%.2f ms)\n",
               s->total_lines, s->total_words, s->total_bytes,
               s->jobs_done, dt);
        shm_destroy(SHM_NAME, s);
        pl_sem_close(m); pl_sem_unlink(SEM_MUTEX);
    }
    else if(!strcmp(mode, "philo")){
        run_philosophers(cfg.nb_philosophers, 5);
    }
    else if(!strcmp(mode, "barber")){
        run_barber(cfg.nb_barber_chairs, cfg.nb_processes * 2);
    }
    else if(!strcmp(mode, "mq")){
        run_mq_demo(&cfg);
    }
    else if(!strcmp(mode, "bench") || !strcmp(mode, "all")){
        BenchResult r[3]; int k = 0;

        /* 1. Séquentiel */
        double t = bench_start();
        long ls = run_sequential(&cfg);
        r[k++] = (BenchResult){"sequential", bench_stop(t), ls, 1};

        /* 2. Multiprocessus */
        t = bench_start();
        long lp = run_multiprocess(&cfg);
        r[k++] = (BenchResult){"multiprocess", bench_stop(t),
                               lp, cfg.nb_processes};

        /* 3. Multithreads */
        sem_t *m = pl_sem_create(SEM_MUTEX, 1);
        SharedStats *s = shm_create(SHM_NAME);
        t = bench_start();
        run_multithread(&cfg, s, m);
        r[k++] = (BenchResult){"multithread", bench_stop(t),
                               s->total_lines, cfg.nb_threads};
        shm_destroy(SHM_NAME, s);
        pl_sem_close(m); pl_sem_unlink(SEM_MUTEX);

        bench_report(r, k, "benchmarks/results.csv");

        if(!strcmp(mode,"all")){
            printf("\n--- Philosophes ---\n");
            run_philosophers(cfg.nb_philosophers, 3);
            printf("\n--- Barbier ---\n");
            run_barber(cfg.nb_barber_chairs, cfg.nb_processes*2);
            printf("\n--- Message Queue ---\n");
            run_mq_demo(&cfg);
        }
    }
    else { usage(argv[0]); return 1; }
    return 0;
}
