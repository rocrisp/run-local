#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

logger_t *global_logger = NULL;

logger_t* logger_create(const char *log_file_path, log_level_t min_level) {
    logger_t *logger = (logger_t *)malloc(sizeof(logger_t));
    if (!logger) {
        return NULL;
    }

    logger->log_file = fopen(log_file_path, "a");
    if (!logger->log_file) {
        free(logger);
        return NULL;
    }

    logger->min_level = min_level;
    logger->log_to_console = 1;
    pthread_mutex_init(&logger->log_mutex, NULL);

    return logger;
}

void logger_destroy(logger_t *logger) {
    if (logger) {
        pthread_mutex_destroy(&logger->log_mutex);
        if (logger->log_file) {
            fclose(logger->log_file);
        }
        free(logger);
    }
}

void logger_log(logger_t *logger, log_level_t level, const char *format, ...) {
    if (!logger || level < logger->min_level) {
        return;
    }

    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

    pthread_mutex_lock(&logger->log_mutex);

    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    va_list args;
    va_start(args, format);

    char message[2048];
    vsnprintf(message, sizeof(message), format, args);

    char log_line[2560];
    snprintf(log_line, sizeof(log_line), "[%s] [%s] %s\n",
             timestamp, level_str[level], message);

    if (logger->log_file) {
        fprintf(logger->log_file, "%s", log_line);
        fflush(logger->log_file);
    }

    if (logger->log_to_console) {
        fprintf(stderr, "%s", log_line);
    }

    va_end(args);
    pthread_mutex_unlock(&logger->log_mutex);
}
