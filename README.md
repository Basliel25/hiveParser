  # HiveParser

/** @brief Simple print helper
* @brief Simple print helper

Concurrent syslog parser in C. Reads a log file, distributes lines across N worker threads, and reports severity counts, error rate, and hourly distribution.

Producer/consumer over a mutex-protected linked-list queue. Clean shutdown via `pthread_cond_broadcast`. Each worker accumulates a local `report_t` — no per-line locking — and flushes to the global result at exit.

---

## How it works

```
main                         workers (N)
 │                               │
 ├─ init queue + spawn workers ──┤
 │                               │
 ├─ fgets → enqueue              ├─ dequeue
 ├─ fgets → enqueue              ├─ parse syslog fields
 ├─ ...                          ├─ classify severity
 │                               └─ accumulate local report
 ├─ set done flag
 ├─ broadcast condvar ───────────┤
 │                               └─ flush report → join
 └─ merge reports → print
```

---

## Build

```sh
make
```

Requires `gcc`, compiled with `-std=gnu11 -pthread`.

## Run

```sh
./hiveParse <logfile> [num_threads]
./hiveParse log_data/Linux_2k.log 8
```

## Docker

```sh
docker build -t hiveparse .
docker run --rm -v $(pwd)/log_data:/app/log_data hiveparse log_data/Linux_2k.log 8
```

---

## Output

```
  ┌─────────────────────────────────┐
  │         HiveParser Report        │
  └─────────────────────────────────┘

  Total entries   117843
  Timespan        Jan 03 07:12:44  →  Jan 31 23:58:01

  Severity
  ─────────────────────────────────
  Error      4201  (3.6%)
  Warn       9840
  Info      103802

  Hourly Distribution
  ─────────────────────────────────
  00h  ████████████░░░░░░░░░░░░░░░░  4821
  01h  ████░░░░░░░░░░░░░░░░░░░░░░░░  2103
  ...
```

---

## Core structs

```c
typedef struct node {
    char        *line;
    struct node *next;
} Node;

typedef struct {
    Node            *head, *tail;
    int              size;
    int              flag;   /* 1 = producer done */
    pthread_mutex_t  lock;
    pthread_cond_t   cond;
} workQueue_t;

typedef struct {
    int     total_log;
    int     errors, warn, info;
    double  error_percentage;
    int     hourly[24];
    time_t  earliest, latest;
} report_t;
```

---

## Status

| Version | |
|---|---|
| v0.1 — single-threaded reader | ✅ |
| v0.2 — syslog parser | ✅ |
| v0.3 — threaded queue | ✅ |
| v0.4 — clean shutdown | ✅ |
| v0.5 — aggregated report | 🔧 |
| v1.0 — stable release | ⬜ |
| v1.1 — named pipe output | ⬜ |
| v2.0 — ncurses TUI | ⬜ |

---

Tested against the [loghub Linux.log](https://github.com/logpai/loghub) dataset — ~117k lines of real syslog data.
