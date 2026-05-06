#ifndef LOGPARSE
#define LOGPARSE
#include <stddef.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>

/**
************************
*** Data Structures ****
************************
 */

/**
* @brief Log entry 
*/
typedef struct log_entry_s {
    char *month;
    char *day;
    char *timestamp;
    char *host;
    char *component;
    char *pid;
    char *message;
    char *severity;
} log_entry_t;

/**
* @brief Queue Node
*/
typedef struct Node {
    char *line; /**< Queue input, line to be parsed */ 
    struct Node *next; /**< Points to next field */ 
} Node;

/**
* @brief Queue 
*/
typedef struct workQueue_s {
    struct Node *head; /**< Points to start of list */
    struct Node *tail; /**< Points to end of list */
    int size; /**< Size of list */
    pthread_mutex_t lock; /**< Thread lock */
    pthread_cond_t cond; /**< Thread condition signal */
    int flag; /**< Done flag, 1 - done, 0 - not done */
} workQueue_t;

/**
* @brief Report Field 
*/
typedef struct report_s {
   int total_log; /**< Amount of total log entries */
   int errors; /**< Amount of errors */
   int warn; /**< Amount of warns */
   int info; /**< Amount of infos */
   int hourly[24]; /**< Hourly ditribution of log */
   double error_percentage; /**< Error Percentage */
   time_t earliest, latest; /** Tracking the latest and earliest time stamp.*/
} report_t;

/**
* @brief Queue 
*/
typedef struct {
    int thread_id;
    workQueue_t *queue;
    report_t thread_report;
} thread_args_t;


/**
****************
*** Modules ****
****************
*/

/**
* @brief Thread function
* @param void *arg the working queue
*/
void enqueue(workQueue_t *queue, char *line);
/**
* @brief Thread function
* @param void *arg the working queue
*/
char *dequeue(workQueue_t *queue);

/**
 * @brief Thread function
 * @param void *arg the working queue
 */
void *worker_thread(void *arg);

/**
* @brief Function to update report field
* @param char message - The message section of a log entry
* @return char severity The severity of the message
*/
void report_update(log_entry_t *entry, report_t *report);
#endif
