# PipeLab++

**Atelier de Traitement Intensif, Parallèle & Benchmarking**
Domaine d'application : **traitement de fichiers texte volumineux**

> Projet — Module *Systèmes d'Exploitation Avancés*
>
> **Binôme :** Melek Ben Mansour & Fares Amdouni

---

## 1. Objectif

PipeLab++ est une application en **C/UNIX** qui traite des fichiers volumineux en
exploitant simultanément :

| Critère du sujet | Implémentation |
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

## 2. Justification — Fidélité au cours

Le **noyau** (fork/exec/pipe/dup2/wait) est intégralement issu du cours fourni :

- `fork()` : slide 90
- `exec*()` recouvrement : slides 102-106, 136
- `pipe()` + `read`/`write` : slides 119-128
- `dup2()` redirection : slide 134
- `wait()`/`waitpid()` : slides 96-99
- Exemple `ls -al | wc -l` : slide 135 (reproduit dans `examples/ex_ls_wc.c`)
- Exemple bidirectionnel 5 entiers ×2 : slide 139 (`examples/ex_bidirectionnel.c`)
- Compilation `gcc` (`-E`, `-S`, `-c`, link) : slides 16-18 (cibles du Makefile)

Les modules **non-couverts** par le cours mais imposés par le sujet du projet
(threads, sémaphores, mémoire partagée, files de messages, philosophes,
barbier, benchmarking) sont implémentés en **modules isolés**, signalés en
en-tête de chaque fichier source.

---

## 3. Architecture

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

## 4. Structure des dossiers

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
│   ├── worker_proc.c           # MULTIPROCESSUS + EXEC + PIPE
│   ├── worker_thread.c         # MULTITHREADS + SHM + SEM
│   ├── sync_philosophers.c     # Philosophes dîneurs
│   ├── sync_barber.c           # Barbier endormi
│   └── ipc_mq_demo.c           # File de messages
├── examples/
│   ├── ex_ls_wc.c              # FIDÈLE slide 135
│   └── ex_bidirectionnel.c     # FIDÈLE slide 139
├── scripts/
│   ├── gen_big_file.sh
│   └── run_bench.sh
└── benchmarks/
    └── results.csv             # sortie du benchmark
```

---

## 5. Compilation

```bash
make                # construit ./pipelab + examples/
make preprocess     # gcc -E (slide 16)
make assemble       # gcc -S (slide 17)
make compile        # gcc -c (slide 18)
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

4. **Compiler et lancer** (commandes Linux habituelles ci-dessous) :
```bash
make
./scripts/gen_big_file.sh 20 data/big.txt   # générer un fichier de 20 Mo
./pipelab proc
./pipelab bench
```

> Astuce : pour de meilleures performances, on peut copier le projet dans le
> système de fichiers Linux (ex. `~/PipeLab`) plutôt que de travailler sous
> `/mnt/c`. Les fonctions `mq_*` (files de messages POSIX) requièrent Linux et
> fonctionnent sous WSL2.

---

## 6. Utilisation

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
sequential,1,327680,42.10,7782898
multiprocess,4,327680,18.55,17665229
multithread,4,327680,12.30,26640650
```

---

## 7. Mapping critères → fichiers

| Critère | Fichier(s) |
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

## 8. Note sur l'équité du benchmark

Le mode `proc` **découpe le fichier en N tranches d'octets distinctes** :
chaque processus fils traite exactement `1/N` du fichier (et non le fichier
entier N fois). Le mode `thread` applique le même principe. La comparaison
`sequential` vs `multiprocess` vs `multithread` porte donc bien sur la **même
quantité de travail**, ce qui rend le benchmark équitable.

Le découpage par octets est **exact pour `wc -l`** : chaque caractère `'\n'`
appartient à une seule tranche, donc la somme des comptes par tranche est
égale au compte total du fichier. (Pour `grep -c`, une ligne coupée à la
frontière d'une tranche peut être comptée de façon approximative — acceptable
pour la démonstration.)

