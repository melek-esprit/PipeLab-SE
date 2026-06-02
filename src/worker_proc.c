/*
 * Module : Multiprocessus + Recouvrement (exec) + Pipe
 * Couvre :
 *   - fork() (cours slide 90)
 *   - exec*() recouvrement (cours slides 102-106, 136)
 *   - pipe + dup2 (cours slides 127-134)
 *   - wait/waitpid (cours slides 96-99)
 *
 * NB : le fichier est ici DECOUPE EN TRANCHES D'OCTETS distinctes : chaque
 * processus fils traite 1/N du fichier (et non le fichier entier N fois).
 * Le découpage par octets est exact pour `wc -l` car chaque '\n' appartient
 * à exactement une tranche ; la somme des tranches = total du fichier.
 * Le recouvrement (exec) est conservé : chaque fils charge des outils UNIX
 * existants (sh, tail, head, wc/grep/sort) via execlp.
 */
#include "common.h"
#include "pipe_utils.h"

/* Taille du fichier en octets (pour le découpage en tranches). */
static long file_size(const char *path){
    FILE *f = fopen(path, "rb");
    if(!f) die("open input");
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fclose(f);
    return sz;
}

/* Construit la commande shell qui extrait la tranche [start, start+len)
 * puis lui applique l'outil existant demandé (recouvrement). */
static void build_slice_cmd(const Config *cfg, long start, long len,
                            char *out, size_t outsz){
    /* tail -c +K = à partir de l'octet K (1-indexé) ; head -c LEN = LEN octets */
    if(strcmp(cfg->command, "grep") == 0){
        snprintf(out, outsz,
                 "tail -c +%ld \"%s\" | head -c %ld | grep -c \"%s\"",
                 start + 1, cfg->input_file, len, cfg->pattern);
    } else if(strcmp(cfg->command, "sort") == 0){
        snprintf(out, outsz,
                 "tail -c +%ld \"%s\" | head -c %ld | sort | wc -l",
                 start + 1, cfg->input_file, len);
    } else { /* wc (defaut) ou commande personnalisée comptant les lignes */
        snprintf(out, outsz,
                 "tail -c +%ld \"%s\" | head -c %ld | wc -l",
                 start + 1, cfg->input_file, len);
    }
}

/* Lance N processus fils ; chacun traite UNE tranche d'octets distincte du
 * fichier via execlp (RECOUVREMENT). Le père récupère le résultat de chaque
 * fils via un tube anonyme et cumule le total.                                */
long run_multiprocess(const Config *cfg){
    int  N    = cfg->nb_processes;
    long size = file_size(cfg->input_file);
    long step = (N > 0) ? size / N : size;
    long total_lines = 0;

    for(int i=0; i<N; i++){
        long start = (long)i * step;
        long end   = (i == N-1) ? size : (long)(i+1) * step;
        long len   = end - start;
        if(len < 0) len = 0;

        int fd[2];
        pl_pipe(fd);                               /* tube anonyme */

        pid_t pid = fork();                        /* fork (slide 90) */
        if(pid < 0) die("fork");

        if(pid == 0){
            /* ---- FILS ---- */
            close(fd[0]);                          /* ferme lecture */
            pl_redirect(fd[1], STDOUT_FILENO);     /* dup2 (slide 134) */

            /* RECOUVREMENT : remplace le code du fils par des commandes
               existantes (sh + tail/head + wc/grep/sort) appliquées à SA
               tranche d'octets — slides 102-106, 136 */
            char cmd[1024];
            build_slice_cmd(cfg, start, len, cmd, sizeof(cmd));
            execlp("sh", "sh", "-c", cmd, (char*)NULL);
            die("execlp");                         /* unreachable si OK */
        } else {
            /* ---- PÈRE ---- */
            close(fd[1]);
            char buf[256];
            ssize_t n = pl_read_line(fd[0], buf, sizeof(buf));
            if(n > 0){
                long v = atol(buf);
                total_lines += v;
                printf("[proc %d/%d pid=%d] %s [octets %ld..%ld] -> %ld\n",
                       i+1, N, pid, cfg->command, start, end, v);
            }
            close(fd[0]);
            int st; waitpid(pid, &st, 0);          /* synchro (slide 96) */
        }
    }
    return total_lines;
}
