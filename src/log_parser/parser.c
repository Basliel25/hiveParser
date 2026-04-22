#include "parser.h"

char *severity(char *message) {
    //Convert the message into lowercase
    for(int i = 0; message[i]; i++)
        message[i] = (unsigned char)tolower(message[i]);

    if(strstr(message, "failure") || 
            strstr(message, "failure"))  {return "Error";}
    if(strstr(message, "authentication failure")) {return "Auth Failure";}
    if(strstr(message, "alert")) {return "alert";}
    if(strstr(message, "warn") ||
            strstr(message, "warning")) {return "Warn";}
    if(strstr(message, "info")) {return "Info";}
    else {return "None";}

    return "/0";
}

log_entry_t *extract_entry(char *line) {
    log_entry_t *log_entry = malloc(sizeof(log_entry_t));

    // Save Pointer
    char *saveptr;
    // Tokenize input
    char *token = strtok_r(line, " ", &saveptr);
    
    // First input is month
    log_entry->month = token;

    // Second input is date
    token = strtok_r(NULL," ", &saveptr);
    log_entry->day = token;

    // Third input is timestamp
    token = strtok_r(NULL," ", &saveptr);
    log_entry->timestamp = token;

    // Fourth input is host
    token = strtok_r(NULL," ", &saveptr);
    log_entry->host = token;

    // Next input is component
    // New delimeter
    token = strtok_r(NULL,"[", &saveptr);
    log_entry->component = token;

    // Next Entry is PID
    token = strtok_r(NULL,"]", &saveptr);
    log_entry->pid = token;

    // Last Entry is message
    token = strtok_r(NULL,":", &saveptr);
    // Severity
    log_entry->message = token;

    if(log_entry->message == NULL)
        log_entry->severity = NULL;
    else {
        char *sever = severity(log_entry->message);
        log_entry->severity = sever;
    }

    return log_entry;
}

