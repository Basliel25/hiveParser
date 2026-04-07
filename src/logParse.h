#ifndef LOGPARSE
#define LOGPARSE

#include <stddef.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
************************
*** Data Structures ****
************************
 */

/**
* @brief Log entry 
*/
typedef struct __log_entry_t {
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
typedef struct __workQueue_t {
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
typedef struct __report_t {
   char timestamp_range; /**< The time range of the log*/
   int total_log; /**< Amount of total log entries */
   int errors; /**< Amount of errors */
   int warn; /**< Amount of warns */
   int info; /**< Amount of infos */
   int error_percentage; /**< Error Percentage */
} report_t;

/**
****************
*** Modules ****
****************
*/

/**
* @brief Thread function
* @param void *arg the working queue
*/
void *worker_thread(void *arg);
/**
 * @brief Function to catagorize severity
 * @param char message - The message section of a log entry
 * @return char severity The severity of the message
 */
char *severity(char *message);

/**
* @brief module to extract details from a parsed line
* @param char *line - Entry from log to be processed
* @return log_entry_t struct with organized data about entry
*/
log_entry_t *extract_entry(char *line);


/**
* @brief Function to update report field
* @param char message - The message section of a log entry
* @return char severity The severity of the message
*/
void report_update(log_entry_t *entry, report_t *report);
#endif
