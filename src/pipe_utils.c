#include "pipe_utils.h"

int pl_pipe(int fd[2]){
    if(pipe(fd) == -1) die("pipe");
    return 0;
}

ssize_t pl_write_all(int fd, const void *buf, size_t n){
    const char *p = buf; size_t left = n;
    while(left){
        ssize_t w = write(fd, p, left);
        if(w < 0){ if(errno==EINTR) continue; return -1; }
        left -= w; p += w;
    }
    return n;
}

ssize_t pl_read_line(int fd, char *buf, size_t n){
    size_t i = 0; char c;
    while(i < n-1){
        ssize_t r = read(fd, &c, 1);
        if(r <= 0) break;
        buf[i++] = c;
        if(c == '\n') break;
    }
    buf[i] = 0;
    return i;
}

void pl_redirect(int from, int to){
    if(dup2(from, to) == -1) die("dup2");
    close(from);
}
