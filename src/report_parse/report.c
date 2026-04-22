#include "report.h"

void init(report_t *report) {
    memset(report, 0, sizeof(report_t));

    report->total_log = 0;
    report->errors = 0;
    report->warn = 0;
    report->info = 0;
    report->error_percentage = 0.0;
    report->earliest = LLONG_MAX;
    report->latest = 0;
}

void merge_report(report_t *master, report_t *thread) {

    master->total_log += thread->total_log;
    master->errors += thread->errors;
    master->warn += thread->warn;
    master->info += thread->info;

    // Calculate error percentage for each thread
    master->error_percentage = error_percentage(master);


     master->earliest = (thread->earliest < master->earliest) ? 
         thread->earliest : master->earliest;
    master->latest = (thread->latest > master->latest) ? thread->latest : master->latest;
}

double error_percentage(report_t *report) {
    return (double) report->errors / (double) report->total_log;
}

void print(report_t *report) {
    double pct = error_percentage(report);
    printf("=== Log Report ===\n");
    printf("Total entries: %d\n", report->total_log);
    printf("Errors: %d (%.1f%%)\n", report->errors, pct);
    printf("Warnings: %d\n", report->warn);
    printf("Info: %d\n", report->info);
    printf("Time range: %ld - %ld\n", report->earliest, report->latest);
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
    if(strstr(entry->severity, "Error")) {thread->errors += 1;}
    if(strstr(entry->severity, "Warn")) {thread->warn += 1;}
    if(strstr(entry->severity, "Info")) {thread->info += 1;}


    // Construct time stamp into time_t to calculate latest and earliest
    time_t timestamp = reconstruct_ts(entry);

     thread->earliest = (timestamp < thread->earliest) ? timestamp : thread->earliest;
    thread->latest = (timestamp > thread->latest) ? timestamp : thread->latest;

    struct tm tm_struct;
    struct tm *tm = localtime_r(&timestamp, &tm_struct);
    int hour = tm->tm_hour;
    thread->hourly[hour]++;
}

