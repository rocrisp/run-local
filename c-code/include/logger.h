#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <pthread.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

typedef struct logger {
    FILE *log_file;
    log_level_t min_level;
    pthread_mutex_t log_mutex;
    int log_to_console;
} logger_t;

logger_t* logger_create(const char *log_file_path, log_level_t min_level);
void logger_destroy(logger_t *logger);
void logger_log(logger_t *logger, log_level_t level, const char *format, ...);

#define LOG(logger, level, ...) logger_log(logger, level, __VA_ARGS__)
#define LOG_DEBUG(logger, ...) LOG(logger, LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(logger, ...)  LOG(logger, LOG_INFO, __VA_ARGS__)
#define LOG_WARN(logger, ...)  LOG(logger, LOG_WARN, __VA_ARGS__)
#define LOG_ERROR(logger, ...) LOG(logger, LOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(logger, ...) LOG(logger, LOG_FATAL, __VA_ARGS__)

extern logger_t *global_logger;

#endif
