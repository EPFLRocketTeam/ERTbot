#ifndef LOG_H 
#define LOG_H

#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_ERROR 2

void log_message(int level, const char *format, ...);


#endif