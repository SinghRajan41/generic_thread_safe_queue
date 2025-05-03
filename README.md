# generic_thread_safe_queue

# Thread-Safe Queue with Timeout and Multiple Operations

## Introduction

This project implements a **thread-safe queue** that supports various operations such as **enqueue**, **dequeue**, **peek**, and **size** with both blocking and non-blocking behaviors. It also supports **timeout-based operations**. The implementation uses C++ threads and synchronization mechanisms like `mutex`, `timed_mutex`, and `condition_variable` to ensure thread safety and manage concurrency.

The queue supports multiple threads (worker threads) performing operations concurrently. The **stress test** demonstrates how the queue handles multiple simultaneous operations, showcasing its performance under concurrent access.

## Features

1. **Thread-Safety:**
   - The queue ensures that concurrent operations on the queue (such as enqueueing, dequeuing, and querying the size) are thread-safe using locks (i.e., `mutex` and `timed_mutex`).

2. **Blocking Enqueue & Dequeue:**
   - **Blocking Enqueue:** The queue will block if it is full until space is available.
   - **Blocking Dequeue:** The queue will block if it is empty until there is an item to pop.

3. **Non-Blocking Operations:**
   - **Try Enqueue:** This operation attempts to enqueue an item but returns `false` if the queue is full or the lock cannot be acquired.
   - **Try Dequeue:** This operation attempts to dequeue an item but returns `false` if the queue is empty or the lock cannot be acquired.
   - **Try Size:** This operation checks the size of the queue without blocking. It returns `-1` if it cannot acquire the lock.

4. **Timeout-Based Operations:**
   - **Enqueue with Timeout:** Attempts to enqueue an item and returns `false` if the operation takes longer than the specified timeout period.
   - **Dequeue with Timeout:** Attempts to dequeue an item and returns `false` if the operation takes longer than the specified timeout period.
   - **Peek with Timeout:** Attempts to peek the front item of the queue and returns `false` if the operation takes longer than the specified timeout period.

5. **Peek Operations:**
   - **Blocking Peek:** This operation blocks if the queue is empty until an item is available at the front.
   - **Non-blocking Peek:** This operation checks the front item without blocking, returning `false` if the queue is empty.

6. **Multiple Worker Threads:**
   - Multiple worker threads can simultaneously operate on the queue, either enqueuing or dequeuing items, performing size queries, or peeking, allowing you to test the concurrency capabilities of the queue.

## How It Works

1. **Thread-Safe Queue Class:**
   - The `thread_safe_queue` class is a template class where operations on the queue (enqueue, dequeue, peek, size) are synchronized using `timed_mutex` to allow multi-threaded access.
   - `condition_variable` is used to notify and wait when necessary (e.g., when an item is added or removed).

2. **Worker Threads:**
   - The `workers` function simulates random queue operations across multiple worker threads. Each thread will randomly perform one of the operations (enqueue, dequeue, size query, peek, etc.).

3. **Stress Testing:**
   - The `stress_test` function starts multiple threads (12 by default), each performing operations on the queue, which is shared among them. This tests the queue’s ability to handle concurrent access.

## Example Usage

To use the `thread_safe_queue` and run the stress test, simply compile and run the program:

1. **Compile the code:**
   ```bash
   g++ -std=c++11 thread_safe_queue.cpp -o thread_safe_queue
