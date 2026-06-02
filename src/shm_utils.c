#include "shm_utils.h"

SharedStats *shm_create(const char *name){
    shm_unlink(name);
    int fd = shm_open(name, O_CREAT|O_RDWR|O_EXCL, 0600);
    if(fd < 0) die("shm_open create");
    if(ftruncate(fd, sizeof(SharedStats)) < 0) die("ftruncate");
    SharedStats *p = mmap(NULL, sizeof(SharedStats),
                          PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(p == MAP_FAILED) die("mmap");
    close(fd);
    memset(p, 0, sizeof(*p));
    return p;
}

SharedStats *shm_open_existing(const char *name){
    int fd = shm_open(name, O_RDWR, 0600);
    if(fd < 0) die("shm_open");
    SharedStats *p = mmap(NULL, sizeof(SharedStats),
                          PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(p == MAP_FAILED) die("mmap");
    close(fd);
    return p;
}

void shm_destroy(const char *name, SharedStats *p){
    munmap(p, sizeof(SharedStats));
    shm_unlink(name);
}
