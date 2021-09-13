#ifndef LOG_H
#define LOG_H


#include <stdio.h>
#include <stdarg.h>


#define log_trace(...)  log_message(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...)  log_message(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)   log_message(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)   log_message(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_err(...)  log_message(log_err, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...)  log_message(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)


enum log_level
{
    LOG_TRACE   = 0,
    LOG_DEBUG   = 1,
    LOG_INFO    = 2,
    LOG_WARN    = 3,
    log_err   = 4,
    LOG_FATAL   = 5,
};


void log_message(enum log_level lvl, const char* file, int line, const char* format, ...);


static const char* log_level_colors[] =
{
    "\033[36m", // trace, cyan
    "\033[34m", // debug, blue
    "\033[32m", // info, green
    "\033[33m", // warn, yellow
    "\033[31m", // error, red
    "\033[35m", // fatal, magenta
};

static const char* log_reset_color = "\033[0m";


static const char* log_level_strings[] = 
{
    "trace",
    "debug",
    "info",
    "warn",
    "error",
    "fatal"
};


void log_message(enum log_level lvl, const char* file, int line, const char* format, ...)
{
    fprintf(stderr, "%s[%s]%s ", log_level_colors[lvl], log_level_strings[lvl], log_reset_color);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    printf("\n");
}



#endif
