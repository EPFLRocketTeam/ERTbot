#include "../include/struct.h"
#include "../include/api.h"
#include "../include/config.h"
#include "../include/features.h"
#include "../include/githubAPI.h"
#include "../include/helperFunctions.h"
#include "../include/markdownToPDF.h"
#include "../include/slackAPI.h"
#include "../include/stringTools.h"
#include "../include/wikiAPI.h"
#include "../include/sheetAPI.h"
#include "../include/command.h"
#include "../include/log.h"

void log_message(int level, const char *format, ...) {
    FILE *log_file;
    const char *filename;
    
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