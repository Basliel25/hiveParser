#ifndef REPORT
#define REPORT
#define _XOPEN_SOURCE
// ANSI helpers
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define DIM     "\033[2m"

#include "../logParse.h"


/** @brief Simple print helper
 */
static void print_bar(int count, int max, int width);

/**
 * @brief Initialize Sentinel values
 * @param report_t report - The report struct to be created
 */
void report_init(report_t *report);

/**
 * @brief Function to merge reports
 * @param report_t master - The master aggregate report
 * @param report_t thread - The thread specific report 
 *                          to be merged.
 */
void merge_report(report_t *master, report_t *thread);

/**
 * @brief Function to update thread reports
 * @param log_entry_t entry - The parsed log entry
 * @param report_t thread - The thread specific report field
 */
void update_report(log_entry_t *entry, report_t *thread);

/**
 * @brief Display report
 * @param report - A report summary to be parsed into stdout
 */
void print_report(report_t *report);

/**
 * @brief Calculates error percentage
 * @param report - A report to calculate error percentage on
 */
double error_percentage(report_t *report);

/**
 * 
 * @brief Function to reconstruct timestamp and convert
 *        it to time_t
 * @param log_entry_t entry 
 * @return time_t reconstructed time
 */
time_t reconstruct_ts(log_entry_t *entry);
#endif
