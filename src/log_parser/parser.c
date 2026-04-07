#include "parser.h"

char *severity(char *message) {
    //Convert the message into lowercase
    for(int i = 0; message[i]; i++)
        message[i] = (unsigned char)tolower(message[i]);

    if(strstr(message, "error"))  {return "Error";}
    if(strstr(message, "authentication failure")) {return "Auth Failure";}
    if(strstr(message, "succee")) {return "Successus";}
    if(strstr(message, "connect")) {return "Connection Successful";}
    if(strstr(message, "alert")) {return "alert";}
    else {return "None";}

    return "/0";
}

log_entry_t *extract_entry(char *line) {
    log_entry_t *log_entry = malloc(sizeof(log_entry_t));

    // Tokenize input
    char *token = strtok(line, " ");
    
    // First input is month
    log_entry->month = token;

    // Second input is date
    token = strtok(NULL," ");
    log_entry->day = token;

    // Third input is timestamp
    token = strtok(NULL," ");
    log_entry->timestamp = token;

    // Fourth input is host
    token = strtok(NULL," ");
    log_entry->host = token;

    // Next input is component
    // New delimeter
    token = strtok(NULL,"[");
    log_entry->component = token;

    // Next Entry is PID
    token = strtok(NULL,"]");
    log_entry->pid = token;

    // Last Entry is PID
    token = strtok(NULL,":");
    log_entry->message = token;

    // Severity
    char *sever = severity(token);
    log_entry->severity = sever;

    return log_entry;
}

