#include "scheduler.h"
#include "logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static void* worker_thread(void *arg);
static task_t* dequeue_task(scheduler_t *scheduler);
static void enqueue_task(scheduler_t *scheduler, task_t *task);
static void execute_task(scheduler_t *scheduler, task_t *task);

scheduler_t* scheduler_create(int num_workers) {
    if (num_workers < 1 || num_workers > MAX_WORKER_THREADS) {
        num_workers = DEFAULT_WORKER_THREADS;
    }

    scheduler_t *scheduler = (scheduler_t *)malloc(sizeof(scheduler_t));
    if (!scheduler) {
        return NULL;
    }

    scheduler->task_queue_head = NULL;
    scheduler->completed_tasks = NULL;
    scheduler->num_workers = num_workers;
    scheduler->running = false;
    scheduler->next_task_id = 1;
    scheduler->tasks_submitted = 0;
    scheduler->tasks_completed = 0;
    scheduler->tasks_failed = 0;

    pthread_mutex_init(&scheduler->queue_mutex, NULL);
    pthread_cond_init(&scheduler->queue_cond, NULL);
    pthread_mutex_init(&scheduler->stats_mutex, NULL);

    LOG_INFO(global_logger, "Scheduler created with %d worker threads", num_workers);

    return scheduler;
}

void scheduler_destroy(scheduler_t *scheduler) {
    if (!scheduler) return;

    scheduler_stop(scheduler);

    task_t *current = scheduler->task_queue_head;
    while (current) {
        task_t *next = current->next;
        task_destroy(current);
        current = next;
    }

    current = scheduler->completed_tasks;
    while (current) {
        task_t *next = current->next;
        task_destroy(current);
        current = next;
    }

    pthread_mutex_destroy(&scheduler->queue_mutex);
    pthread_cond_destroy(&scheduler->queue_cond);
    pthread_mutex_destroy(&scheduler->stats_mutex);

    free(scheduler);
    LOG_INFO(global_logger, "Scheduler destroyed");
}

int scheduler_submit_task(scheduler_t *scheduler, task_t *task) {
    if (!scheduler || !task) {
        return -1;
    }

    pthread_mutex_lock(&scheduler->queue_mutex);

    task->id = scheduler->next_task_id++;
    enqueue_task(scheduler, task);

    pthread_mutex_lock(&scheduler->stats_mutex);
    scheduler->tasks_submitted++;
    pthread_mutex_unlock(&scheduler->stats_mutex);

    pthread_cond_signal(&scheduler->queue_cond);
    pthread_mutex_unlock(&scheduler->queue_mutex);

    LOG_INFO(global_logger, "Task submitted: ID=%llu, Name=%s, Priority=%d",
             task->id, task->name, task->priority);

    return 0;
}

void scheduler_start(scheduler_t *scheduler) {
    if (!scheduler || scheduler->running) {
        return;
    }

    scheduler->running = true;

    for (int i = 0; i < scheduler->num_workers; i++) {
        pthread_create(&scheduler->worker_threads[i], NULL, worker_thread, scheduler);
    }

    LOG_INFO(global_logger, "Scheduler started with %d workers", scheduler->num_workers);
}

void scheduler_stop(scheduler_t *scheduler) {
    if (!scheduler || !scheduler->running) {
        return;
    }

    scheduler->running = false;
    pthread_cond_broadcast(&scheduler->queue_cond);

    for (int i = 0; i < scheduler->num_workers; i++) {
        pthread_join(scheduler->worker_threads[i], NULL);
    }

    LOG_INFO(global_logger, "Scheduler stopped");
}

void scheduler_wait_completion(scheduler_t *scheduler) {
    if (!scheduler) return;

    while (1) {
        pthread_mutex_lock(&scheduler->queue_mutex);
        int has_tasks = (scheduler->task_queue_head != NULL);
        pthread_mutex_unlock(&scheduler->queue_mutex);

        if (!has_tasks) {
            break;
        }

        usleep(100000);
    }

    usleep(500000);
}

void scheduler_print_stats(const scheduler_t *scheduler) {
    if (!scheduler) return;

    pthread_mutex_lock((pthread_mutex_t *)&scheduler->stats_mutex);

    printf("\n=== Scheduler Statistics ===\n");
    printf("Tasks Submitted:  %llu\n", scheduler->tasks_submitted);
    printf("Tasks Completed:  %llu\n", scheduler->tasks_completed);
    printf("Tasks Failed:     %llu\n", scheduler->tasks_failed);
    printf("Success Rate:     %.2f%%\n",
           scheduler->tasks_submitted > 0 ?
           (100.0 * scheduler->tasks_completed / scheduler->tasks_submitted) : 0.0);
    printf("===========================\n\n");

    pthread_mutex_unlock((pthread_mutex_t *)&scheduler->stats_mutex);
}

static void* worker_thread(void *arg) {
    scheduler_t *scheduler = (scheduler_t *)arg;
    pthread_t tid = pthread_self();

    LOG_INFO(global_logger, "Worker thread %lu started", (unsigned long)tid);

    while (scheduler->running) {
        pthread_mutex_lock(&scheduler->queue_mutex);

        while (scheduler->running && scheduler->task_queue_head == NULL) {
            pthread_cond_wait(&scheduler->queue_cond, &scheduler->queue_mutex);
        }

        if (!scheduler->running) {
            pthread_mutex_unlock(&scheduler->queue_mutex);
            break;
        }

        task_t *task = dequeue_task(scheduler);
        pthread_mutex_unlock(&scheduler->queue_mutex);

        if (task) {
            execute_task(scheduler, task);
        }
    }

    LOG_INFO(global_logger, "Worker thread %lu stopped", (unsigned long)tid);
    return NULL;
}

static task_t* dequeue_task(scheduler_t *scheduler) {
    if (!scheduler->task_queue_head) {
        return NULL;
    }

    task_t *highest_priority = scheduler->task_queue_head;
    task_t *current = scheduler->task_queue_head->next;
    task_t *prev = scheduler->task_queue_head;
    task_t *highest_prev = NULL;

    while (current) {
        if (current->priority > highest_priority->priority) {
            highest_priority = current;
            highest_prev = prev;
        }
        prev = current;
        current = current->next;
    }

    if (highest_prev) {
        highest_prev->next = highest_priority->next;
    } else {
        scheduler->task_queue_head = highest_priority->next;
    }

    highest_priority->next = NULL;
    return highest_priority;
}

static void enqueue_task(scheduler_t *scheduler, task_t *task) {
    task->next = scheduler->task_queue_head;
    scheduler->task_queue_head = task;
}

static void execute_task(scheduler_t *scheduler, task_t *task) {
    LOG_INFO(global_logger, "Executing task: ID=%llu, Name=%s", task->id, task->name);

    task->status = TASK_RUNNING;
    clock_gettime(CLOCK_MONOTONIC, &task->started_at);

    if (task->task_function) {
        task->result = task->task_function(task->task_args);
    }

    clock_gettime(CLOCK_MONOTONIC, &task->completed_at);

    if (task->result == (void *)-1) {
        task->status = TASK_FAILED;
        pthread_mutex_lock(&scheduler->stats_mutex);
        scheduler->tasks_failed++;
        pthread_mutex_unlock(&scheduler->stats_mutex);
        LOG_ERROR(global_logger, "Task failed: ID=%llu, Name=%s", task->id, task->name);
    } else {
        task->status = TASK_COMPLETED;
        pthread_mutex_lock(&scheduler->stats_mutex);
        scheduler->tasks_completed++;
        pthread_mutex_unlock(&scheduler->stats_mutex);
        LOG_INFO(global_logger, "Task completed: ID=%llu, Name=%s, Time=%.3fs",
                 task->id, task->name, task_execution_time(task));
    }

    pthread_mutex_lock(&scheduler->queue_mutex);
    task->next = scheduler->completed_tasks;
    scheduler->completed_tasks = task;
    pthread_mutex_unlock(&scheduler->queue_mutex);
}
