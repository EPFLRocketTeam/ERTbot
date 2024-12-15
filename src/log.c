#include <stdio.h>
#include <stdarg.h>
#include "ERTbot_common.h"


// Function to log messages with different log levels
void log_message(int level, const char *func, const char *format, ...) {
    FILE *log_file;
    const char *filename;

    #ifndef DEBUG
    if (level == LOG_DEBUG) {
        return;
    }
    #endif

    switch(level) {
        case LOG_DEBUG:
            filename = "logs/debug.log";
            break;
        case LOG_INFO:
            filename = "logs/info.log";
            break;
        case LOG_ERROR:
            filename = "logs/error.log";
            break;
        default:
            return;
    }

    log_file = fopen(filename, "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    time_t now = time(NULL);
    const struct tm *tm_info = localtime(&now);
    fprintf(log_file, "[%02d-%02d-%d %02d:%02d:%02d] ",
            tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);

    fprintf(log_file, "[%s] ", func);

    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);

    fprintf(log_file, "\n");
    fclose(log_file);
}

void log_function_entry(const char *func) {
    log_message(LOG_DEBUG, func, "Entering function");
}

void log_function_exit(const char *func) {
    log_message(LOG_DEBUG, func, "Exiting function");
}