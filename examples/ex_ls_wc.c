/*
 * Exemple FIDÈLE au cours, slide 135 :
 *   ls -al | wc -l   via fork + pipe + dup2 + execlp
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(void){
    int fd[2]; pipe(fd);
    if(fork() == 0){               /* fils 1 : ls -al */
        dup2(fd[1], 1); close(fd[0]); close(fd[1]);
        execlp("ls", "ls", "-al", (char*)NULL);
    }
    if(fork() == 0){               /* fils 2 : wc -l  */
        dup2(fd[0], 0); close(fd[0]); close(fd[1]);
        execlp("wc", "wc", "-l", (char*)NULL);
    }
    close(fd[0]); close(fd[1]);
    wait(NULL); wait(NULL);
    return 0;
}
