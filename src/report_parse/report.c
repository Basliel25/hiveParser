#include "report.h"

void report_init(report_t *report) {
    memset(report, 0, sizeof(report_t));
    report->earliest = LLONG_MAX;
}

void merge_report(report_t *master, report_t *thread) {

    master->total_log += thread->total_log;
    master->errors += thread->errors;
    master->warn += thread->warn;
    master->info += thread->info;

    for(int i = 0; i < 24; i++) {
        master->hourly[i] += thread->hourly[i]; 
    }
    master->earliest = (thread->earliest < master->earliest) ? 
        thread->earliest : master->earliest;
    master->latest = (thread->latest > master->latest) ? thread->latest : master->latest;
}

double error_percentage(report_t *report) {
    return (double) report->errors / (double) report->total_log;
}

void print_report(report_t *report) {
    double pct = error_percentage(report);
    printf("=== Log Report ===\n");
    printf("Total entries: %d\n", report->total_log);
    printf("Errors: %d (%.1f%%)\n", report->errors, pct);
    printf("Warnings: %d\n", report->warn);
    printf("Info: %d\n", report->info);

    struct tm *tm = localtime(&report->earliest);
    printf("Spanning from : %04d-%02d-%02d %02d:%02d:%02d\n",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    struct tm *tm1 = localtime(&report->latest);
    printf("To: %04d-%02d-%02d %02d:%02d:%02d\n",
            tm1->tm_year + 1900, tm1->tm_mon + 1, tm1->tm_mday,
            tm1->tm_hour, tm1->tm_min, tm1->tm_sec);

    printf("Hourly distribution:\n");
    for(int i = 0; i < 24; i++) {
        printf("  Hour %02d: %d\n", i, report->hourly[i]);
    }
}

time_t reconstruct_ts(log_entry_t *entry) {
    // Merging time data into one singular time stamp
    char full_ts[64];
    snprintf(full_ts, sizeof(full_ts), "%s %s %s", entry->month, entry->day, entry->timestamp);

    struct tm tm = {0};
    strptime(full_ts, "%b %d %H:%M:%S", &tm);
    tm.tm_year = 2026 - 1900;  
    time_t t = mktime(&tm);
    return t;
}
void update_report(log_entry_t *entry, report_t *thread) {

    thread->total_log += 1;
    
    // Handle severity reports
    if(entry->severity != NULL){
        if(strstr(entry->severity, "Error")) {thread->errors += 1;}
        if(strstr(entry->severity, "Warn")) {thread->warn += 1;}
        if(strstr(entry->severity, "Info")) {thread->info += 1;}
    }

    // Construct time stamp into time_t to calculate latest and earliest
    time_t timestamp = reconstruct_ts(entry);

     thread->earliest = (timestamp < thread->earliest) ? timestamp : thread->earliest;
    thread->latest = (timestamp > thread->latest) ? timestamp : thread->latest;

    struct tm tm_struct;
    struct tm *tm = localtime_r(&timestamp, &tm_struct);
    int hour = tm->tm_hour;
    thread->hourly[hour]++;
}

