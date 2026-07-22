#include "task.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

task_t* task_create(const char *name, const char *description,
                    task_priority_t priority,
                    void *(*func)(void *), void *args) {
    task_t *task = (task_t *)malloc(sizeof(task_t));
    if (!task) {
        return NULL;
    }

    task->id = 0;
    strncpy(task->name, name, MAX_TASK_NAME - 1);
    task->name[MAX_TASK_NAME - 1] = '\0';

    strncpy(task->description, description, MAX_TASK_DESC - 1);
    task->description[MAX_TASK_DESC - 1] = '\0';

    task->priority = priority;
    task->status = TASK_PENDING;
    task->task_function = func;
    task->task_args = args;
    task->result = NULL;

    clock_gettime(CLOCK_MONOTONIC, &task->created_at);
    task->started_at.tv_sec = 0;
    task->started_at.tv_nsec = 0;
    task->completed_at.tv_sec = 0;
    task->completed_at.tv_nsec = 0;

    task->retry_count = 0;
    task->max_retries = 3;
    task->next = NULL;

    return task;
}

void task_destroy(task_t *task) {
    if (task) {
        free(task);
    }
}

void task_print(const task_t *task) {
    if (!task) return;

    const char *status_str[] = {
        "PENDING", "RUNNING", "COMPLETED", "FAILED", "CANCELLED"
    };

    printf("Task ID: %llu\n", task->id);
    printf("  Name: %s\n", task->name);
    printf("  Description: %s\n", task->description);
    printf("  Priority: %d\n", task->priority);
    printf("  Status: %s\n", status_str[task->status]);
    printf("  Retry Count: %d/%d\n", task->retry_count, task->max_retries);

    if (task->status == TASK_COMPLETED || task->status == TASK_FAILED) {
        printf("  Execution Time: %.3f seconds\n", task_execution_time(task));
    }
}

double task_execution_time(const task_t *task) {
    if (!task || task->started_at.tv_sec == 0) {
        return 0.0;
    }

    double start = task->started_at.tv_sec + task->started_at.tv_nsec / 1e9;
    double end = task->completed_at.tv_sec + task->completed_at.tv_nsec / 1e9;

    return end - start;
}
