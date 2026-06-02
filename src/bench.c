#include "bench.h"

double bench_start(void){ return now_ms(); }
double bench_stop (double t0){ return now_ms() - t0; }

void bench_report(const BenchResult *r, int n, const char *outfile){
    FILE *f = fopen(outfile, "w");
    if(!f){ perror("bench_report"); return; }
    fprintf(f, "label,workers,items,elapsed_ms,throughput_items_per_s\n");
    printf("\n========== BENCHMARK ==========\n");
    printf("%-22s %-8s %-10s %-12s %-12s\n",
           "label","workers","items","elapsed_ms","items/s");
    for(int i=0;i<n;i++){
        double thr = r[i].items / (r[i].elapsed_ms/1000.0 + 1e-9);
        printf("%-22s %-8d %-10ld %-12.2f %-12.0f\n",
               r[i].label, r[i].nb_workers, r[i].items,
               r[i].elapsed_ms, thr);
        fprintf(f, "%s,%d,%ld,%.2f,%.0f\n",
                r[i].label, r[i].nb_workers, r[i].items,
                r[i].elapsed_ms, thr);
    }
    printf("================================\n");
    fclose(f);
}
