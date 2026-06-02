/*
 * Exemple FIDÈLE au cours, slide 139 :
 *   Le père envoie 5 entiers au fils via un tube.
 *   Le fils multiplie chacun par 2 et renvoie via un second tube.
 */
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(void){
    int p2c[2], c2p[2];
    pipe(p2c); pipe(c2p);

    if(fork() == 0){
        close(p2c[1]); close(c2p[0]);
        int v;
        for(int i=0;i<5;i++){
            read(p2c[0], &v, sizeof(int));
            v *= 2;
            write(c2p[1], &v, sizeof(int));
        }
        close(p2c[0]); close(c2p[1]);
        return 0;
    }
    close(p2c[0]); close(c2p[1]);
    for(int i=1;i<=5;i++) write(p2c[1], &i, sizeof(int));
    int r;
    for(int i=0;i<5;i++){
        read(c2p[0], &r, sizeof(int));
        printf("Reçu du fils : %d\n", r);
    }
    close(p2c[1]); close(c2p[0]);
    wait(NULL);
    return 0;
}
