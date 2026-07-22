#ifndef TASK_H
#define TASK_H

#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define MAX_TASK_NAME 256
#define MAX_TASK_DESC 1024

typedef enum {
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 5,
    PRIORITY_HIGH = 10,
    PRIORITY_CRITICAL = 20
} task_priority_t;

typedef enum {
    TASK_PENDING,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_CANCELLED
} task_status_t;

typedef struct task {
    uint64_t id;
    char name[MAX_TASK_NAME];
    char description[MAX_TASK_DESC];
    task_priority_t priority;
    task_status_t status;

    void *(*task_function)(void *);
    void *task_args;
    void *result;

    struct timespec created_at;
    struct timespec started_at;
    struct timespec completed_at;

    int retry_count;
    int max_retries;

    struct task *next;
} task_t;

task_t* task_create(const char *name, const char *description,
                    task_priority_t priority,
                    void *(*func)(void *), void *args);
void task_destroy(task_t *task);
void task_print(const task_t *task);
double task_execution_time(const task_t *task);

#endif
