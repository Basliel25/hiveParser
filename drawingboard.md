# Log Parser
>Parsing log output on N threads
A simple log parser that runs on a queue using N concurrent threads.
**Log:** `loghub Linux` - A collection of `/var/log/messages` on a linux server over a period of 260+ days.

### What it does
- The main thread reeads `log` and pushes to a shared queue
- The main spawns *N* worker threads
- Each *N*th workr reads from queue:
  - Parse log entries
- Threads will be be coordinated with *mutex* and *condition variables*.
- Pipe parsed data to:
  - `stdout`.
  - A named pipe OR a *FIFO*. #v2.0
- Gracefully shutdowns
  - Worker drains queue the exits
### Input
- [Loghub Linux Log](https://github.com/logpai/loghub/tree/master/Linux)
### Core flow
###### Main Worker
- Spins N worker threads
- Until all log entry is parse
    - Reads enty from the log file
    - Flags that there is still work to be dnoe
- Pushes log data to working queue
    - Locks queue
    - Wakes up workers when queue is not empty.
- Unlocks queue
###### Child Workers
- Sleep while queue is empty -> Woken up by main when queue is populated
- Read log entry from working queue
- Run parsing function
- Append parsed information to Result field
- When main signals completeion and done flag is set, exit gracefully.
### Components 
### Data Structures
###### Working Queue
###### Thread Arguments
###### Parsed Data
### Worker Architecture 
### File Organization

### Version controls
| Version | Changes |
|---------|---------|
| **v0.0** | Setup headers and Makefile |
