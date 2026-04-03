# Log Parser
>Parsing log output on N threads

A simple log parser that runs on a queue using N concurrent threads.
**Log:** `loghub Linux` - A collection of `/var/log/messages` on a linux server over a period of 260+ days.

### What it does
- The main thread reeads `log` and pushes to a shared queue
- The main spawns *N* worker threads
- Each *N*th workr reads from queue:
  - Parse log entries, for each log entry extract:
    - Day, month and time
    - Host, PID and component
    - Severity and message
  - Infer the following from all log entries.
    - Range of timestamp
    - Severity of errors - total count of errors and warns #v0.1
    - Error percentage - Total count of errors #v0.1
    - Time distribution - Which hour was the busiest #v0.2
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
###### Main module
- Main module
```C
/**
* @brief main function
* @return Exit code - 0
*/
int main();
```

###### Thread module
```C
/**
* @brief Thread running function
* @param void *arg the working queue
*/
void *worker_thread(void *arg);
```
###### Parser modules
 
 - Extract details about the file
```C
/**
* @brief module to extract details from a parsed line
* @param char *line - Entry from log to be processed
* @return log_entry_t struct with organized data about entry
*/

log_entry_t extract_entry(char *line);
```
###### Helper modules
- Catagorize severity

```C
/**
* @brief Function to catagorize severity
* @param char message - The message section of a log entry
* @return char severity The severity of the message
*/

char severity(char message);
```

- Update Report field

```C
/**
* @brief Function to update report field
* @param char message - The message section of a log entry
* @return char severity The severity of the message
*/

void report_update(log_entry_t *entry, report_t *report);
```


### Data Structures

- Number of threads to use
```C
static const int NUMTHREADS;
```

###### Working Queue
 A dynamically growing queue, implemented using a linked list.
- Linked list node:
```C
typedef struct Node {
    char *line; /**< Queue input, line to be parsed */ 
    struct Node *next; /**< Points to next field */ 
} Node;
```
- Working Queue
```C
typedef struct __workQueue_t {
    struct Node *head; /**< Points to start of list */
    struct Node *tail; /**< Points to end of list */
    int size; /**< Size of list */
    pthread_mutex_lock lock; /**< Thread lock */
    pthread_cond_t cond; /**< Thread condition signal */
    int flag; /**< Done flag, 1 - done, 0 - not done */
} workQueue_t;
```

###### Parsed Data
Multi-field struct to hold parsed data
```C
typedef __report_t {
   char timestamp_range; /**< The time range of the log*/
   int total_log; /**< Amount of total log entries */
   int errors; /**< Amount of errors */
   int warn; /**< Amount of warns */
   int info; /**< Amount of infos */
   int error_percentage; /**< Error Percentage */
} report;
```
- A log entry
```C
typedef struct __log_entry_t {
    char month[4];
    int day;
    char time[9];
    char host[64];
    char component[64];
    int pid;
    char message[256];
    char severity[64];
} log_entry_t;
```
- 
### Worker Responsiblity/ownership 
### File Organization

### Version controls
| Version | Changes |
|---------|---------|
| **v0.0** | Setup headers and Makefile |
