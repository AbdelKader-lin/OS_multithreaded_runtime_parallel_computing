# Multithreaded Task Runtime in C

A small parallel runtime built in C with POSIX threads as part of an operating systems project. The goal was to progressively build a task execution system and improve how work is distributed between worker threads.

## What the project implements

The runtime uses a pool of worker threads to execute tasks concurrently. The implementation was developed in several stages, starting from a simple shared task queue and moving toward multiple queues with finer-grained synchronization and work stealing.

The final version includes:

* a configurable pool of POSIX threads
* task queues associated with worker threads
* round-robin task dispatch
* mutex-based synchronization for concurrent queue access
* condition variables for coordination between workers and the main thread
* dynamic queue resizing
* task dependency tracking
* work stealing to redistribute work when a worker has no local task

## Project structure

`stage_1` to `stage_4` contain the successive versions of the runtime. Keeping these stages makes it possible to follow the evolution from the initial implementation to the final parallel scheduler.

The main runtime implementation is in `stage_4`.

Important files include:

* `tasks.c` and `tasks.h`: task creation and execution logic
* `tasks_implem.c`: thread pool, task dispatch and task lifecycle
* `tasks_queue.c`: concurrent task queues and work stealing
* `parallel_for.c`: parallel loop support
* `Makefile.config`: runtime configuration, including the number of worker threads

The repository also contains the original lab specification and project report.

## Build and run

From the final stage:

```bash
cd stage_4
make
```

The exact executable and test commands depend on the programs enabled by the Makefile. `THREAD_COUNT` in `Makefile.config` controls the number of worker threads used by the runtime.

## Concepts explored

This project was mainly an exercise in systems and parallel programming. It covers POSIX threads, mutexes, condition variables, producer-consumer synchronization, task scheduling, load balancing, work stealing and synchronization of task dependencies.

## Context

Developed during the MOSIG operating systems coursework at Université Grenoble Alpes. The repository keeps the intermediate stages to document how the runtime evolved during the lab.