#include "report.h"

static void print_bar(int count, int max, int width) {
    int filled = (max > 0) ? (count * width / max) : 0;
    for (int i = 0; i < filled; i++)  printf("█");
    for (int i = filled; i < width;  i++) printf("░");
}

void print_report(report_t *report) {
    double pct = error_percentage(report);

    char start_buf[32], end_buf[32];
    struct tm tm_s, tm_e;
    localtime_r(&report->earliest, &tm_s);
    localtime_r(&report->latest,   &tm_e);
    strftime(start_buf, sizeof(start_buf), "%b %d %H:%M:%S", &tm_s);
    strftime(end_buf,   sizeof(end_buf),   "%b %d %H:%M:%S", &tm_e);

    // ── header ───────────────────────────────────────────────────────────────
    printf("\n");
    printf(BOLD "  ┌─────────────────────────────────┐\n" RESET);
    printf(BOLD "  │         HiveParser Report        │\n" RESET);
    printf(BOLD "  └─────────────────────────────────┘\n" RESET);
    printf("\n");

    // ── summary ──────────────────────────────────────────────────────────────
    printf("  Total entries   %d\n", report->total_log);
    printf("  Timespan        %s  →  %s\n", start_buf, end_buf);
    printf("\n");

    // ── severity ─────────────────────────────────────────────────────────────
    printf(BOLD "  Severity\n" RESET);
    printf(DIM  "  ─────────────────────────────────\n" RESET);
    printf(RED    "  Error    %6d  (%.1f%%)\n" RESET, report->errors, pct * 100.0);
    printf(YELLOW "  Warn     %6d\n" RESET, report->warn);
    printf(CYAN   "  Info     %6d\n" RESET, report->info);
    printf("\n");

    // ── hourly distribution ──────────────────────────────────────────────────
    int max_hour = 0;
    for (int i = 0; i < 24; i++)
        if (report->hourly[i] > max_hour) max_hour = report->hourly[i];

    printf(BOLD "  Hourly Distribution\n" RESET);
    printf(DIM  "  ─────────────────────────────────\n" RESET);
    for (int i = 0; i < 24; i++) {
        printf("  %02dh  ", i);
        print_bar(report->hourly[i], max_hour, 28);
        printf(DIM "  %d\n" RESET, report->hourly[i]);
    }
    printf("\n");
}

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

