#include <stdio.h>
#include <stdarg.h>
#include "ERTbot_common.h"


void log_message(int level, const char *format, ...) {
    FILE *log_file;
    const char *filename;

    // If debug logging is disabled, skip debug messages
    #ifndef DEBUG
    if (level == LOG_DEBUG) {
        return;
    }
    #endif
    
    // Set the file name based on log level
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
    
    // Log timestamp
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    fprintf(log_file, "[%02d-%02d-%d %02d:%02d:%02d] ",
            tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    
    // Log message
    va_list args;
    va_start(args, format);
    vfprintf(log_file, format, args);
    va_end(args);
    
    fprintf(log_file, "\n");
    fclose(log_file);
}