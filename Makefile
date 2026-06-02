# ===========================================================
# PipeLab++ — Build (cours slides 16-18 : gcc -E / -S / -c / link)
# ===========================================================
CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -Iinclude -D_GNU_SOURCE -pthread
LDFLAGS := -pthread -lrt

SRC := src/main.c src/common.c src/pipe_utils.c src/shm_utils.c \
       src/sem_utils.c src/mq_utils.c src/thread_pool.c src/bench.c \
       src/worker_proc.c src/worker_thread.c \
       src/sync_philosophers.c src/sync_barber.c src/ipc_mq_demo.c

OBJ := $(SRC:.c=.o)

all: pipelab examples

pipelab: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

examples: examples/ex_ls_wc examples/ex_bidirectionnel

examples/ex_ls_wc: examples/ex_ls_wc.c
	$(CC) $(CFLAGS) -o $@ $<

examples/ex_bidirectionnel: examples/ex_bidirectionnel.c
	$(CC) $(CFLAGS) -o $@ $<

# Étapes de compilation séparées (pédagogique, slides 16-18)
preprocess: src/main.c
	$(CC) -E $(CFLAGS) src/main.c -o build/main.i
assemble:   src/main.c
	$(CC) -S $(CFLAGS) src/main.c -o build/main.s
compile:    src/main.c
	$(CC) -c $(CFLAGS) src/main.c -o build/main.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) pipelab examples/ex_ls_wc examples/ex_bidirectionnel
	rm -rf build benchmarks/results.csv

.PHONY: all clean examples preprocess assemble compile
