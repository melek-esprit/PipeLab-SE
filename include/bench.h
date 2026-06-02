#ifndef BENCH_H
#define BENCH_H
#include "common.h"

typedef struct {
    const char *label;
    double      elapsed_ms;
    long        items;
    int         nb_workers;
} BenchResult;

double bench_start(void);
double bench_stop (double t0);
void   bench_report(const BenchResult *r, int n, const char *outfile);

#endif
