# Multi-Threaded Task Scheduler

A sophisticated multi-threaded task scheduler implementation in C demonstrating advanced programming concepts.

## Features

### Core Architecture
- **Priority-based task scheduling** - Tasks executed based on priority levels (LOW, NORMAL, HIGH, CRITICAL)
- **Multi-threaded worker pool** - Configurable number of worker threads (1-16)
- **Thread-safe operations** - Using mutexes and condition variables
- **Comprehensive logging** - Thread-safe logging to file and console
- **Configuration management** - External config file support
- **Task statistics** - Real-time tracking of task execution metrics

### Advanced Concepts Demonstrated

1. **Multi-Threading & Synchronization**
   - POSIX threads (pthreads)
   - Mutex locks for critical sections
   - Condition variables for thread signaling
   - Thread-safe data structures

2. **Memory Management**
   - Dynamic memory allocation
   - Proper cleanup and deallocation
   - No memory leaks

3. **Data Structures**
   - Priority queue implementation
   - Linked lists
   - Task queue management

4. **Modular Design**
   - Separate header and source files
   - Clean API interfaces
   - Encapsulation

5. **Error Handling**
   - Comprehensive error checking
   - Graceful failure handling
   - Status tracking

6. **File I/O**
   - Configuration file parsing
   - Log file management
   - Thread-safe file operations

## Project Structure

```
c-code/
├── include/           # Header files
│   ├── task.h        # Task structure and operations
│   ├── scheduler.h   # Scheduler interface
│   ├── logger.h      # Logging system
│   └── config.h      # Configuration management
├── src/              # Source files
│   ├── task.c        # Task implementation
│   ├── scheduler.c   # Scheduler implementation
│   ├── logger.c      # Logger implementation
│   ├── config.c      # Config parser
│   └── main.c        # Main application
├── obj/              # Object files (generated)
├── bin/              # Binary output (generated)
├── logs/             # Log files (generated)
├── config/           # Configuration files
│   └── scheduler.conf
├── Makefile          # Build system
└── README.md         # This file
```

## Building

### Prerequisites
- GCC compiler with C11 support
- POSIX threads library (pthread)
- Math library (libm)
- Make

### Build Commands

```bash
# Build the project
make

# Build with debug symbols
make debug

# Clean build artifacts
make clean

# Build and run
make run

# Show help
make help
```

## Usage

### Basic Usage
```bash
./bin/scheduler
```

### With Custom Config
```bash
./bin/scheduler path/to/config.conf
```

### Example Configuration
Edit `config/scheduler.conf`:

```ini
num_workers = 4        # Number of worker threads
max_retries = 3        # Max retry attempts for failed tasks
log_to_console = 1     # Enable console logging
log_file_path = logs/scheduler.log
log_level = 1          # 0=DEBUG, 1=INFO, 2=WARN, 3=ERROR, 4=FATAL
```

## Example Tasks

The demo application includes:

1. **Fibonacci Computation** - Calculate Fibonacci numbers
2. **Prime Number Counting** - Count primes up to N
3. **Factorial Calculation** - Compute factorials
4. **Square Root Calculation** - Mathematical operations
5. **I/O Simulation** - Simulate blocking I/O operations

## Output Example

```
╔════════════════════════════════════════╗
║  Multi-Threaded Task Scheduler v1.0   ║
║  Advanced C Programming Demo          ║
╚════════════════════════════════════════╝

=== Configuration ===
Number of Workers: 4
Max Retries:       3
Log to Console:    1
Log File Path:     logs/scheduler.log
Log Level:         1
====================

Submitting tasks...

Waiting for all tasks to complete...

All tasks completed!

=== Scheduler Statistics ===
Tasks Submitted:  23
Tasks Completed:  23
Tasks Failed:     0
Success Rate:     100.00%
===========================
```

## Code Highlights

### Task Priority System
```c
typedef enum {
    PRIORITY_LOW = 1,
    PRIORITY_NORMAL = 5,
    PRIORITY_HIGH = 10,
    PRIORITY_CRITICAL = 20
} task_priority_t;
```

### Thread-Safe Logging
```c
LOG_INFO(global_logger, "Task completed: ID=%llu, Name=%s, Time=%.3fs",
         task->id, task->name, task_execution_time(task));
```

### Worker Thread Pool
```c
for (int i = 0; i < scheduler->num_workers; i++) {
    pthread_create(&scheduler->worker_threads[i], NULL, worker_thread, scheduler);
}
```

## Technical Details

### Synchronization Primitives
- **Queue Mutex**: Protects task queue access
- **Condition Variable**: Signals workers when tasks are available
- **Stats Mutex**: Protects statistics updates
- **Log Mutex**: Ensures thread-safe logging

### Task States
- TASK_PENDING - Waiting in queue
- TASK_RUNNING - Currently executing
- TASK_COMPLETED - Successfully finished
- TASK_FAILED - Execution failed
- TASK_CANCELLED - Manually cancelled

### Performance Considerations
- Lock granularity optimized
- Minimal critical sections
- Priority-based scheduling
- Efficient queue operations

## Extensibility

### Adding New Task Types
1. Define task function with signature: `void* (*task_function)(void *)`
2. Create task: `task_create(name, desc, priority, function, args)`
3. Submit: `scheduler_submit_task(scheduler, task)`

### Custom Task Example
```c
void* my_custom_task(void *arg) {
    // Your task logic here
    LOG_INFO(global_logger, "Custom task executing");
    
    // Return result or (void*)-1 on failure
    return result;
}
```

## Error Handling

- All memory allocations checked
- Proper error propagation
- Graceful degradation
- Comprehensive logging

## Memory Safety

- No memory leaks (verify with valgrind)
- All allocated memory freed
- Thread-safe memory operations
- Proper cleanup on shutdown

## Logging

Logs are written to both:
- Console (if enabled in config)
- Log file: `logs/scheduler.log`

Log format:
```
[2026-07-21 14:30:45] [INFO] Task completed: ID=5, Name=Fibonacci-20, Time=0.102s
```

## Testing

```bash
# Build and run
make run

# Run with valgrind to check for memory leaks
valgrind --leak-check=full ./bin/scheduler

# Run with custom config
./bin/scheduler config/custom.conf
```

## Limitations & Future Enhancements

### Current Limitations
- Fixed maximum worker threads (16)
- Simple priority queue (not heap-based)
- No task dependencies
- No task cancellation API

### Potential Enhancements
- Heap-based priority queue for O(log n) operations
- Task dependency graph
- Dynamic thread pool scaling
- Task cancellation support
- Persistent task queue (database)
- REST API for task submission
- Real-time monitoring dashboard
- Task result caching

## Educational Value

This project demonstrates:
- ✅ Multi-threaded programming
- ✅ Synchronization primitives
- ✅ Thread-safe data structures
- ✅ Memory management
- ✅ Modular C design
- ✅ Build systems (Make)
- ✅ Configuration management
- ✅ Logging systems
- ✅ Error handling
- ✅ Code organization

## License

Educational/demonstration code - free to use and modify.

## Author

Created as a demonstration of advanced C programming concepts.
