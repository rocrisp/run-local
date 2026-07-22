#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "scheduler.h"
#include "logger.h"
#include "config.h"

void* compute_fibonacci(void *arg) {
    int n = *(int *)arg;

    if (n < 0) {
        return (void *)-1;
    }

    uint64_t *result = malloc(sizeof(uint64_t));
    if (!result) {
        return (void *)-1;
    }

    if (n <= 1) {
        *result = n;
    } else {
        uint64_t a = 0, b = 1, temp;
        for (int i = 2; i <= n; i++) {
            temp = a + b;
            a = b;
            b = temp;
        }
        *result = b;
    }

    usleep(100000);

    LOG_INFO(global_logger, "Fibonacci(%d) = %llu", n, *result);
    return result;
}

void* compute_prime_count(void *arg) {
    int n = *(int *)arg;

    if (n < 2) {
        return (void *)-1;
    }

    int *count = malloc(sizeof(int));
    if (!count) {
        return (void *)-1;
    }

    *count = 0;
    for (int num = 2; num <= n; num++) {
        int is_prime = 1;
        for (int i = 2; i * i <= num; i++) {
            if (num % i == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) {
            (*count)++;
        }
    }

    usleep(200000);

    LOG_INFO(global_logger, "Prime count up to %d = %d", n, *count);
    return count;
}

void* compute_factorial(void *arg) {
    int n = *(int *)arg;

    if (n < 0 || n > 20) {
        return (void *)-1;
    }

    uint64_t *result = malloc(sizeof(uint64_t));
    if (!result) {
        return (void *)-1;
    }

    *result = 1;
    for (int i = 2; i <= n; i++) {
        *result *= i;
    }

    usleep(50000);

    LOG_INFO(global_logger, "Factorial(%d) = %llu", n, *result);
    return result;
}

void* compute_square_root(void *arg) {
    double *num = (double *)arg;

    if (*num < 0) {
        return (void *)-1;
    }

    double *result = malloc(sizeof(double));
    if (!result) {
        return (void *)-1;
    }

    *result = sqrt(*num);

    usleep(30000);

    LOG_INFO(global_logger, "Square root of %.2f = %.6f", *num, *result);
    return result;
}

void* simulate_io_operation(void *arg) {
    int delay_ms = *(int *)arg;

    LOG_INFO(global_logger, "Starting I/O simulation for %d ms", delay_ms);
    usleep(delay_ms * 1000);

    int *success = malloc(sizeof(int));
    if (!success) {
        return (void *)-1;
    }

    *success = 1;
    LOG_INFO(global_logger, "I/O simulation completed");
    return success;
}

const int MAX_MATRIX_SIZE = 100;
typedef struct {
    double data[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE];
    int rows;
    int cols;
} matrix_t;

void* compute_matrix_multiplication(void *arg) {
    matrix_t *matrices = (matrix_t *)arg;
    matrix_t *result = malloc(sizeof(matrix_t));
    if (!result) {
        return (void *)-1;
    }

    matrix_t *m1 = &matrices[0];
    matrix_t *m2 = &matrices[1];
    
    if (m1->cols != m2->rows) {
        result->rows = -1;
        result->cols = -1;
        return result;
    }

    result->rows = m1->rows;
    result->cols = m2->cols;

    for (int i = 0; i < m1->rows; i++) {
        for (int j = 0; j < m2->cols; j++) {
            result->data[i][j] = 0.0;
            for (int k = 0; k < m1->cols; k++) {
                result->data[i][j] += m1->data[i][k] * m2->data[k][j];
            }
        }
    }

    usleep(150000);

    LOG_INFO(global_logger, "Matrix multiplication completed: (%d x %d) * (%d x %d) = (%d x %d)",
             m1->rows, m1->cols, m2->rows, m2->cols, result->rows, result->cols);
    return result;
}

int main(int argc, char *argv[]) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║  Multi-Threaded Task Scheduler v1.0   ║\n");
    printf("║  Advanced C Programming Demo          ║\n");
    printf("╚════════════════════════════════════════╝\n\n");

    const char *config_file = "config/scheduler.conf";
    if (argc > 1) {
        config_file = argv[1];
    }

    config_t *config = config_load(config_file);
    config_print(config);

    global_logger = logger_create(config->log_file_path, config->log_level);
    if (!global_logger) {
        fprintf(stderr, "Failed to create logger\n");
        config_destroy(config);
        return 1;
    }

    LOG_INFO(global_logger, "=== Task Scheduler Starting ===");

    scheduler_t *scheduler = scheduler_create(config->num_workers);
    if (!scheduler) {
        fprintf(stderr, "Failed to create scheduler\n");
        logger_destroy(global_logger);
        config_destroy(config);
        return 1;
    }

    scheduler_start(scheduler);

    printf("Submitting tasks...\n\n");

    int fib_args[] = {10, 20, 15, 25, 30, 5};
    for (int i = 0; i < 6; i++) {
        int *arg = malloc(sizeof(int));
        *arg = fib_args[i];

        char name[64], desc[256];
        snprintf(name, sizeof(name), "Fibonacci-%d", fib_args[i]);
        snprintf(desc, sizeof(desc), "Calculate Fibonacci number at position %d", fib_args[i]);

        task_t *task = task_create(name, desc, PRIORITY_NORMAL, compute_fibonacci, arg);
        scheduler_submit_task(scheduler, task);
    }

    int prime_args[] = {100, 500, 1000, 50};
    for (int i = 0; i < 4; i++) {
        int *arg = malloc(sizeof(int));
        *arg = prime_args[i];

        char name[64], desc[256];
        snprintf(name, sizeof(name), "PrimeCount-%d", prime_args[i]);
        snprintf(desc, sizeof(desc), "Count prime numbers up to %d", prime_args[i]);

        task_t *task = task_create(name, desc, PRIORITY_HIGH, compute_prime_count, arg);
        scheduler_submit_task(scheduler, task);
    }

    int fact_args[] = {5, 10, 15, 20};
    for (int i = 0; i < 4; i++) {
        int *arg = malloc(sizeof(int));
        *arg = fact_args[i];

        char name[64], desc[256];
        snprintf(name, sizeof(name), "Factorial-%d", fact_args[i]);
        snprintf(desc, sizeof(desc), "Calculate factorial of %d", fact_args[i]);

        task_t *task = task_create(name, desc, PRIORITY_LOW, compute_factorial, arg);
        scheduler_submit_task(scheduler, task);
    }

    double sqrt_args[] = {16.0, 81.0, 144.0, 256.0, 625.0};
    for (int i = 0; i < 5; i++) {
        double *arg = malloc(sizeof(double));
        *arg = sqrt_args[i];

        char name[64], desc[256];
        snprintf(name, sizeof(name), "SquareRoot-%.0f", sqrt_args[i]);
        snprintf(desc, sizeof(desc), "Calculate square root of %.0f", sqrt_args[i]);

        task_t *task = task_create(name, desc, PRIORITY_NORMAL, compute_square_root, arg);
        scheduler_submit_task(scheduler, task);
    }

    int io_args[] = {500, 1000, 250, 750};
    for (int i = 0; i < 4; i++) {
        int *arg = malloc(sizeof(int));
        *arg = io_args[i];

        char name[64], desc[256];
        snprintf(name, sizeof(name), "IOSimulation-%dms", io_args[i]);
        snprintf(desc, sizeof(desc), "Simulate I/O operation for %d milliseconds", io_args[i]);

        task_t *task = task_create(name, desc, PRIORITY_CRITICAL, simulate_io_operation, arg);
        scheduler_submit_task(scheduler, task);
    }

    matrix_t matrices[2];
    matrix_t *m1 = &matrices[0];
    m1->rows = 2;
    m1->cols = 3;
    matrix_t *m2 = &matrices[1];
    m2->rows = 3;
    m2->cols = 2;
    matrices[0].data[0][0] = 1.0; matrices[0].data[0][1] = 2.0; matrices[0].data[0][2] = 3.0;
    matrices[0].data[1][0] = 4.0; matrices[0].data[1][1] = 5.0; matrices[0].data[1][2] = 6.0;
    matrices[1].data[0][0] = 7.0; matrices[1].data[0][1] = 8.0;
    matrices[1].data[1][0] = 9.0; matrices[1].data[1][1] = 10.0;
    matrices[1].data[2][0] = 11.0; matrices[1].data[2][1] = 12.0;

    char name[64], desc[256];
    snprintf(name, sizeof(name), "MatrixMult-%dx%d", m1->rows, m1->cols);
    snprintf(desc, sizeof(desc), "Multiply %dx%d matrix by %dx%d matrix", m1->rows, m1->cols, m2->rows, m2->cols);

    task_t *task = task_create(name, desc, PRIORITY_NORMAL, compute_matrix_multiplication, &matrices);
    scheduler_submit_task(scheduler, task);

    printf("Waiting for all tasks to complete...\n");
    scheduler_wait_completion(scheduler);

    printf("\nAll tasks completed!\n");
    scheduler_print_stats(scheduler);

    LOG_INFO(global_logger, "=== Task Scheduler Shutting Down ===");

    scheduler_stop(scheduler);
    scheduler_destroy(scheduler);
    logger_destroy(global_logger);
    config_destroy(config);

    printf("Scheduler shut down successfully.\n");
    printf("Check logs/scheduler.log for detailed execution logs.\n\n");

    return 0;
}
