
# Log Parser
>Parsing log output on N threads
A simple log parser that runs on a queue using N concurrent threads.
**Log:** `loghub Linux` - A collection of `/var/log/messages` on a linux server over a period of 260+ days.

### What it does
- The main thread reads `log` and pushes to a shared queue
- The main spawns *N* worker threads
- Each *N*th worker reads from queue:
  - Parse log entries
- Threads will be coordinated with *mutex* and *condition variables*.
- Gracefully shutdowns
  - Worker drains queue the exits
### Core flow
### Components 
### Data Structures
### Worker Architecture 
### File Organization
