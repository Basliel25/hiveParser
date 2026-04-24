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

void extract_entry(char *line, log_entry_t *entry) {
    // Save Pointer
    char *saveptr;
    // Tokenize input
    char *token = strtok_r(line, " ", &saveptr);
    
    // First input is month
    entry->month = token;

    // Second input is date
    token = strtok_r(NULL," ", &saveptr);
    entry->day = token;

    // Third input is timestamp
    token = strtok_r(NULL," ", &saveptr);
    entry->timestamp = token;

    // Fourth input is host
    token = strtok_r(NULL," ", &saveptr);
    entry->host = token;

    // Next input is component
    // New delimeter
    token = strtok_r(NULL,"[", &saveptr);
    entry->component = token;

    // Next Entry is PID
    token = strtok_r(NULL,"]", &saveptr);
    entry->pid = token;

    // Last Entry is message
    token = strtok_r(NULL,":", &saveptr);
    // Severity
    entry->message = token;

    if(entry->message == NULL)
        entry->severity = NULL;
    else {
        char *sever = severity(entry->message);
        entry->severity = sever;
    }
}

