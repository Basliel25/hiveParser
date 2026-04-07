#ifndef PARSER
#define PARSER
#include "../logParse.h"

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
#endif
