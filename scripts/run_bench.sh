#!/usr/bin/env bash
# Lance un benchmark en faisant varier le nombre de processus & threads
set -e
make
mkdir -p benchmarks
RESULTS=benchmarks/sweep.csv
echo "mode,workers,elapsed_ms" > $RESULTS

for N in 1 2 4 8 16; do
    sed -i.bak "s/^nb_processes = .*/nb_processes = $N/" config/config.ini
    sed -i.bak "s/^nb_threads = .*/nb_threads = $N/"     config/config.ini

    T1=$(./pipelab proc   2>/dev/null | grep -oE '[0-9]+\.[0-9]+ ms' | tail -1 | awk '{print $1}')
    T2=$(./pipelab thread 2>/dev/null | grep -oE '[0-9]+\.[0-9]+ ms' | tail -1 | awk '{print $1}')
    echo "proc,$N,$T1"   >> $RESULTS
    echo "thread,$N,$T2" >> $RESULTS
done
echo "Résultats dans $RESULTS"
