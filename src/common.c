#include "common.h"

double now_ms(void){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000.0 + tv.tv_usec/1000.0;
}

void die(const char *msg){ perror(msg); exit(EXIT_FAILURE); }

static char *trim(char *s){
    while(*s==' '||*s=='\t') s++;
    char *e = s + strlen(s) - 1;
    while(e>s && (*e==' '||*e=='\n'||*e=='\r'||*e=='\t')) *e-- = 0;
    return s;
}

int load_config(const char *path, Config *cfg){
    /* defaults */
    cfg->nb_processes = 4;
    cfg->nb_threads   = 4;
    strcpy(cfg->input_file, "data/big.txt");
    strcpy(cfg->command, "wc");
    strcpy(cfg->pattern, "the");
    cfg->chunk_size       = 1000;
    cfg->nb_philosophers  = 5;
    cfg->nb_barber_chairs = 3;

    FILE *f = fopen(path,"r");
    if(!f){ fprintf(stderr,"[config] %s introuvable, valeurs par défaut\n", path); return 0; }
    char line[256];
    while(fgets(line,sizeof(line),f)){
        if(line[0]=='#'||line[0]=='\n') continue;
        char *eq = strchr(line,'=');
        if(!eq) continue;
        *eq = 0;
        char *k = trim(line), *v = trim(eq+1);
        if      (!strcmp(k,"nb_processes"))     cfg->nb_processes     = atoi(v);
        else if (!strcmp(k,"nb_threads"))       cfg->nb_threads       = atoi(v);
        else if (!strcmp(k,"input_file"))       strncpy(cfg->input_file,v,MAX_PATH-1);
        else if (!strcmp(k,"command"))          strncpy(cfg->command,v,63);
        else if (!strcmp(k,"pattern"))          strncpy(cfg->pattern,v,127);
        else if (!strcmp(k,"chunk_size"))       cfg->chunk_size       = atoi(v);
        else if (!strcmp(k,"nb_philosophers"))  cfg->nb_philosophers  = atoi(v);
        else if (!strcmp(k,"nb_barber_chairs")) cfg->nb_barber_chairs = atoi(v);
    }
    fclose(f);
    return 1;
}
