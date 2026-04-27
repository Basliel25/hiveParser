# HiveParser

Read More on: [Here](https://basz-website.basgug25.workers.dev/projects/hive-parser/)

A concurrent syslog parser in written from scratch in C. The program parses a provided log data, and spawns multiple workers that concurrently parse the log from a shared work pull.
Producer/consumer architecture is managed over a mutex-protected linked-list queue. Work is signaled via `pthread_cond_broadcast`. Each worker accumulates a local `report_t` and flushes to the global result at exit.

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
Tested against the [loghub Linux.log](https://github.com/logpai/loghub) dataset — ~117k lines of real syslog data.
