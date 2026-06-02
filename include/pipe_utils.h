#ifndef PIPE_UTILS_H
#define PIPE_UTILS_H
#include "common.h"

/* IPC #1 : Tubes anonymes (cours slides 119-135) */
int  pl_pipe(int fd[2]);
ssize_t pl_write_all(int fd, const void *buf, size_t n);
ssize_t pl_read_line(int fd, char *buf, size_t n);
void pl_redirect(int from, int to);   /* dup2 (slide 134) */

#endif
