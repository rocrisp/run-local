#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

config_t* config_load(const char *config_file) {
    config_t *config = (config_t *)malloc(sizeof(config_t));
    if (!config) {
        return NULL;
    }

    config->num_workers = 4;
    config->max_retries = 3;
    config->log_to_console = 1;
    strcpy(config->log_file_path, "logs/scheduler.log");
    config->log_level = 1;

    FILE *file = fopen(config_file, "r");
    if (!file) {
        fprintf(stderr, "Warning: Could not open config file %s, using defaults\n", config_file);
        return config;
    }

    char line[MAX_CONFIG_LINE];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        char key[MAX_CONFIG_LINE];
        char value[MAX_CONFIG_LINE];

        if (sscanf(line, "%s = %s", key, value) == 2) {
            if (strcmp(key, "num_workers") == 0) {
                config->num_workers = atoi(value);
            } else if (strcmp(key, "max_retries") == 0) {
                config->max_retries = atoi(value);
            } else if (strcmp(key, "log_to_console") == 0) {
                config->log_to_console = atoi(value);
            } else if (strcmp(key, "log_file_path") == 0) {
                strncpy(config->log_file_path, value, MAX_CONFIG_LINE - 1);
            } else if (strcmp(key, "log_level") == 0) {
                config->log_level = atoi(value);
            }
        }
    }

    fclose(file);
    return config;
}

void config_destroy(config_t *config) {
    if (config) {
        free(config);
    }
}

void config_print(const config_t *config) {
    if (!config) return;

    printf("\n=== Configuration ===\n");
    printf("Number of Workers: %d\n", config->num_workers);
    printf("Max Retries:       %d\n", config->max_retries);
    printf("Log to Console:    %d\n", config->log_to_console);
    printf("Log File Path:     %s\n", config->log_file_path);
    printf("Log Level:         %d\n", config->log_level);
    printf("====================\n\n");
}
