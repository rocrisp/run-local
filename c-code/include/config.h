#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CONFIG_LINE 256

typedef struct config {
    int num_workers;
    int max_retries;
    int log_to_console;
    char log_file_path[MAX_CONFIG_LINE];
    int log_level;
} config_t;

config_t* config_load(const char *config_file);
void config_destroy(config_t *config);
void config_print(const config_t *config);

#endif
