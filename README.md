# PipeLab++

**Atelier de Traitement Intensif, Parallèle & Benchmarking**
Domaine d'application : **traitement de fichiers texte volumineux**

> Projet — Module *Systèmes d'Exploitation Avancés*
>
> **Binôme :** Melek Ben Mansour & Fares Amdouni
>
> **Dépôt :** https://github.com/melek-esprit/PipeLab

---

## 1. Objectif

PipeLab++ est une application en **C/UNIX** qui traite des fichiers volumineux en
exploitant simultanément :

| Fonctionnalité | Implémentation |
|---|---|
| **Multiprocessus** (nb configurable) | `fork()` × N (config.ini) |
| **Multithreads** (nb configurable)   | Pool de threads `pthread` |
| **Recouvrement** de logiciels existants | `execlp("wc"/"grep"/"sort", ...)` |
| **IPC — Pipe**             | Tubes anonymes + `dup2` |
| **IPC — Mémoire partagée** | `shm_open` + `mmap` (POSIX) |
| **IPC — Files de messages** | `mq_open` / `mq_send` / `mq_receive` |
| **Synchronisation — Sémaphores** | `sem_open`, `sem_wait`, `sem_post` |
| **Philosophes dîneurs** | `sync_philosophers.c` (Dijkstra) |
| **Barbier endormi**     | `sync_barber.c` |
| **Benchmarking**        | Module `bench.c`, sortie CSV |

---

## 2. Architecture

```
                       ┌──────────────────────────┐
                       │   main.c (orchestrateur) │
                       │   lecture config.ini     │
                       └──────┬─────────┬─────────┘
              ┌───────────────┘         └───────────────┐
              ▼                                         ▼
   ┌────────────────────┐                   ┌────────────────────┐
   │  worker_proc.c     │                   │  worker_thread.c   │
   │  fork + pipe +     │                   │  pthread pool +    │
   │  execlp + wait     │                   │  shm + sémaphore   │
   └─────────┬──────────┘                   └─────────┬──────────┘
             │                                        │
             ▼                                        ▼
   ┌────────────────────┐                   ┌────────────────────┐
   │  ipc_mq_demo.c     │                   │ sync_philosophers  │
   │  mq_open/send/recv │                   │ sync_barber        │
   └────────────────────┘                   └────────────────────┘
                          ┌───────────────┐
                          │   bench.c     │  → benchmarks/results.csv
                          └───────────────┘
```

---

## 3. Structure des dossiers

```
PipeLab/
├── Makefile
├── README.md
├── config/
│   └── config.ini              # nb_processes, nb_threads, command, ...
├── data/
│   └── big.txt                 # généré par scripts/gen_big_file.sh
├── include/
│   ├── common.h                # Config, helpers
│   ├── pipe_utils.h            # IPC pipe
│   ├── shm_utils.h             # IPC mémoire partagée
│   ├── mq_utils.h              # IPC files de messages
│   ├── sem_utils.h             # sémaphores POSIX
│   ├── thread_pool.h           # pool de threads
│   └── bench.h                 # benchmark
├── src/
│   ├── main.c                  # orchestrateur + modes
│   ├── common.c
│   ├── pipe_utils.c
│   ├── shm_utils.c
│   ├── sem_utils.c
│   ├── mq_utils.c
│   ├── thread_pool.c
│   ├── bench.c
│   ├── worker_proc.c           # multiprocessus + exec + pipe
│   ├── worker_thread.c         # multithreads + shm + sémaphore
│   ├── sync_philosophers.c     # philosophes dîneurs
│   ├── sync_barber.c           # barbier endormi
│   └── ipc_mq_demo.c           # file de messages
├── examples/
│   ├── ex_ls_wc.c              # pipe : ls | wc -l
│   └── ex_bidirectionnel.c     # pipe bidirectionnel
├── scripts/
│   ├── gen_big_file.sh
│   └── run_bench.sh
└── benchmarks/
    └── results.csv             # sortie du benchmark
```

---

## 4. Compilation

```bash
make                # construit ./pipelab + examples/
make preprocess     # gcc -E (préprocesseur)
make assemble       # gcc -S (assembleur)
make compile        # gcc -c (compilation objet)
make clean
```

Dépendances : `gcc`, `libc`, `librt` (pour `mq_*` et `shm_*`), `pthread`.

### Sous Windows : compiler via WSL (Windows Subsystem for Linux)

Le projet utilise des appels POSIX (`fork`, `pipe`, `shm_open`, `mq_*`,
`pthread`, sémaphores) **indisponibles nativement sous Windows**. Sous Windows,
on passe par **WSL** (une vraie distribution Linux intégrée à Windows).

1. **Installer WSL** (PowerShell en administrateur, une seule fois) :
```powershell
wsl --install -d Ubuntu
```
Redémarrer si demandé, puis créer un utilisateur Linux au premier lancement.

2. **Installer les outils de build** (dans le terminal Ubuntu/WSL) :
```bash
sudo apt update
sudo apt install -y build-essential
```

3. **Ouvrir le projet depuis WSL.** Le disque Windows est monté sous `/mnt`.
Le dossier du projet devient :
```bash
cd "/mnt/c/Users/melek/OneDrive/Bureau/Melek/Fac/Esprit/Projet SE/PipeLab"
```

4. **Compiler et lancer** :
```bash
make
./scripts/gen_big_file.sh 20 data/big.txt   # générer un fichier de 20 Mo
./pipelab proc
./pipelab bench
```

> Astuce : pour de meilleures performances, copier le projet dans le système de
> fichiers Linux (ex. `~/PipeLab`) plutôt que de travailler sous `/mnt/c`.
> Les files de messages POSIX (`mq_*`) requièrent Linux / WSL2.

---

## 5. Utilisation

### a) Préparer un fichier volumineux
```bash
chmod +x scripts/gen_big_file.sh
./scripts/gen_big_file.sh 20 data/big.txt    # 20 Mo
```

### b) Configurer
Éditer `config/config.ini` (modifie nb_processes, nb_threads, etc.).

### c) Lancer un mode
```bash
./pipelab proc      # multiprocessus (fork + exec + pipe)
./pipelab thread    # multithreads + shm + sémaphore
./pipelab philo     # philosophes dîneurs
./pipelab barber    # barbier endormi
./pipelab mq        # file de messages
./pipelab bench     # benchmark séquentiel vs proc vs thread
./pipelab all       # tout
```

### d) Benchmark complet (variation du nombre de workers)
```bash
chmod +x scripts/run_bench.sh
./scripts/run_bench.sh
```

Les résultats sont écrits dans `benchmarks/results.csv` :
```
label,workers,items,elapsed_ms,throughput_items_per_s
sequential,1,313008,11018.55,28407
multiprocess,4,313008,753.00,415679
multithread,4,313008,4021.06,77842
```

---

## 6. Mapping fonctionnalités → fichiers

| Fonctionnalité | Fichier(s) |
|---|---|
| Multiprocessus configurable          | `src/worker_proc.c` + `config/config.ini` |
| Multithreads configurable            | `src/worker_thread.c` + `src/thread_pool.c` |
| Recouvrement (`exec`)                | `src/worker_proc.c` (`execlp`) |
| IPC — Pipe                           | `src/pipe_utils.c`, `src/worker_proc.c` |
| IPC — Mémoire partagée               | `src/shm_utils.c`, `src/worker_thread.c` |
| IPC — Queue (files de messages)      | `src/mq_utils.c`, `src/ipc_mq_demo.c` |
| Sémaphores (synchro)                 | `src/sem_utils.c` |
| Philosophes dîneurs                  | `src/sync_philosophers.c` |
| Barbier endormi                      | `src/sync_barber.c` |
| Benchmarking                         | `src/bench.c`, `scripts/run_bench.sh` |

---

## 7. Note sur l'équité du benchmark

Le mode `proc` **découpe le fichier en N tranches d'octets distinctes** :
chaque processus fils traite exactement `1/N` du fichier (et non le fichier
entier N fois). Le mode `thread` applique le même principe. La comparaison
`sequential` vs `multiprocess` vs `multithread` porte donc sur la **même
quantité de travail**, ce qui rend le benchmark équitable.

Le découpage par octets est **exact pour `wc -l`** : chaque caractère `'\n'`
appartient à une seule tranche, donc la somme des comptes par tranche est
égale au compte total du fichier.
