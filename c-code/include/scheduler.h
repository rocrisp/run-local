#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"
#include <pthread.h>
#include <stdbool.h>

#define MAX_WORKER_THREADS 16
#define DEFAULT_WORKER_THREADS 4

typedef struct scheduler {
    task_t *task_queue_head;
    task_t *completed_tasks;

    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_cond;

    pthread_t worker_threads[MAX_WORKER_THREADS];
    int num_workers;

    bool running;
    uint64_t next_task_id;

    uint64_t tasks_submitted;
    uint64_t tasks_completed;
    uint64_t tasks_failed;

    pthread_mutex_t stats_mutex;
} scheduler_t;

scheduler_t* scheduler_create(int num_workers);
void scheduler_destroy(scheduler_t *scheduler);
int scheduler_submit_task(scheduler_t *scheduler, task_t *task);
void scheduler_start(scheduler_t *scheduler);
void scheduler_stop(scheduler_t *scheduler);
void scheduler_wait_completion(scheduler_t *scheduler);
void scheduler_print_stats(const scheduler_t *scheduler);

#endif
