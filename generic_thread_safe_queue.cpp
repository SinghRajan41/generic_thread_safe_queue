#include <bits/stdc++.h>
#define NUM_THREADS 12  // Number of worker threads to run in parallel
using namespace std;

// Template class for a thread-safe queue
template<class X>
class thread_safe_queue {
private:
    queue<X> q;  // The underlying queue to store the data
    const int MAX_SIZE = 1024;  // Maximum size of the queue
    timed_mutex mtx;  // Mutex to ensure thread safety during enqueue, dequeue, and size operations
    condition_variable cv;  // Condition variable to wait on and notify when the queue is not empty

public:
    // Default constructor
    thread_safe_queue() {}

    // Copy constructor for deep copying the queue
    thread_safe_queue(const thread_safe_queue& other) {
        // Lock the mutex to prevent race conditions during copying
        lock_guard<timed_mutex> lock(other.mtx);
        q = other.q;
    }

    // Non-blocking try to enqueue an item. Returns true if successful, false if the queue is full
    bool try_enqueue(X new_item) {
        if (mtx.try_lock()) {
            if (q.size() == MAX_SIZE) {
                mtx.unlock();  // Unlock and return false if the queue is full
                return false;
            } else {
                q.push(new_item);  // Enqueue the new item
                mtx.unlock();  // Unlock the mutex after enqueueing
                return true;
            }
        } else {
            return false;  // Return false if the mutex is not available
        }
    }

    // Blocking enqueue. This will block if the queue is full until space is available.
    bool enqueue_blocking(X new_item) {
        mtx.lock();  // Lock the mutex to ensure thread safety
        if (q.size() == MAX_SIZE) {
            mtx.unlock();  // Unlock if the queue is full
            return false;
        } else {
            q.push(new_item);  // Enqueue the new item
            mtx.unlock();  // Unlock the mutex after enqueueing
            return true;
        }
    }

    // Enqueue with a timeout. Attempts to enqueue and returns false if timeout occurs.
    bool enqueue_blocking_with_timeout(X new_item, chrono::milliseconds timeout_period) {
        unique_lock<timed_mutex> lk(mtx, defer_lock);
        if (lk.try_lock_for(timeout_period)) {  // Try to lock the mutex for the specified timeout
            if (q.size() == MAX_SIZE) {
                lk.unlock();  // Unlock if the queue is full
                return false;
            } else {
                q.push(new_item);  // Enqueue the new item
                lk.unlock();  // Unlock after enqueueing
                return true;
            }
        } else {
            return false;  // Return false if unable to lock within timeout
        }
    }

    // Non-blocking size check. Returns -1 if unable to lock the mutex
    int try_size() {
        if (mtx.try_lock()) {
            int sz = q.size();  // Get the size of the queue
            mtx.unlock();  // Unlock after getting the size
            return sz;
        } else {
            return -1;  // Return -1 if unable to lock the mutex
        }
    }

    // Blocking size check. Returns the size of the queue after acquiring the lock
    int size_blocking() {
        mtx.lock();  // Lock the mutex to ensure thread safety
        int sz = q.size();  // Get the size of the queue
        mtx.unlock();  // Unlock after getting the size
        return sz;
    }

    // Size check with timeout. Returns -1 if timeout occurs.
    int size_blocking_with_timeout(chrono::milliseconds timeout_period) {
        unique_lock<timed_mutex> lk(mtx, defer_lock);
        if (lk.try_lock_for(timeout_period)) {  // Try to lock the mutex for the specified timeout
            int sz = q.size();  // Get the size of the queue
            return sz;
        } else {
            return -1;  // Return -1 if unable to lock within timeout
        }
    }

    // Non-blocking pop. Returns true if the item is successfully popped, otherwise false.
    bool try_pop() {
        if (mtx.try_lock()) {
            if (q.size()) {
                q.pop();  // Pop the front item from the queue
                mtx.unlock();  // Unlock after popping
                return true;
            } else {
                mtx.unlock();  // Unlock if the queue is empty
                return false;
            }
        } else {
            return false;  // Return false if unable to lock the mutex
        }
    }

    // Blocking pop. This will block if the queue is empty until an item is available.
    bool pop_blocking() {
        mtx.lock();  // Lock the mutex to ensure thread safety
        if (q.size()) {
            q.pop();  // Pop the front item from the queue
            mtx.unlock();  // Unlock after popping
            return true;
        } else {
            mtx.unlock();  // Unlock if the queue is empty
            return false;
        }
    }

    // Pop with timeout. Attempts to pop and returns false if timeout occurs.
    bool pop_blocking_with_timeout(chrono::milliseconds timeout_period) {
        unique_lock<timed_mutex> lk(mtx, defer_lock);
        if (lk.try_lock_for(timeout_period)) {  // Try to lock the mutex for the specified timeout
            if (q.size()) {
                q.pop();  // Pop the front item from the queue
                return true;
            } else {
                return false;  // Return false if the queue is empty
            }
        } else {
            return false;  // Return false if unable to lock within timeout
        }
    }

    // Non-blocking peek. Returns true if an item is at the front, otherwise false.
    bool try_peek(X &front_item) {
        if (mtx.try_lock()) {
            if (q.size()) {
                front_item = q.front();  // Get the front item
                mtx.unlock();  // Unlock after peeking
                return true;
            } else {
                mtx.unlock();  // Unlock if the queue is empty
                return false;
            }
        } else {
            return false;  // Return false if unable to lock the mutex
        }
    }

    // Blocking peek. This will block if the queue is empty until an item is available at the front.
    bool peek_blocking(X &front_item) {
        mtx.lock();  // Lock the mutex to ensure thread safety
        if (q.size()) {
            front_item = q.front();  // Get the front item
            mtx.unlock();  // Unlock after peeking
            return true;
        } else {
            mtx.unlock();  // Unlock if the queue is empty
            return false;
        }
    }

    // Peek with timeout. Attempts to peek and returns false if timeout occurs.
    bool peek_blocking_with_timeout(X &front_item, chrono::milliseconds timeout_period) {
        unique_lock<timed_mutex> lk(mtx, defer_lock);
        if (lk.try_lock_for(timeout_period)) {  // Try to lock the mutex for the specified timeout
            if (q.size()) {
                front_item = q.front();  // Get the front item
                lk.unlock();  // Unlock after peeking
                return true;
            } else {
                lk.unlock();  // Unlock if the queue is empty
                return false;
            }
        } else {
            return false;  // Return false if unable to lock within timeout
        }
    }
};

// Worker function that simulates random operations on the thread-safe queue
void workers(thread_safe_queue<int>& q) {
    for (int i = 0; i < 100000; i++) {
        int type = rand() % 10;  // Randomly choose an operation type (0-9)
        cout << type << " ";
        if (type == 0) {
            // Enqueue (blocking)
            int item = rand();
            if (q.enqueue_blocking(item)) {
                cout << "BLOCKING ENQUEUE = " << item << "\n";
            } else {
                cout << "QUEUE FULL\n";
            }
        } else if (type == 1) {
            // Try Enqueue (non-blocking)
            int item = rand();
            if (q.try_enqueue(item)) {
                cout << "NON BLOCKING ENQUEUE = " << item << "\n";
            } else {
                cout << "COULD NOT ENQUEUE DUE TO CONTENTION\n";
            }
        } else if (type == 2) {
            // Enqueue with timeout (blocking)
            int item = rand();
            if (q.enqueue_blocking_with_timeout(item, chrono::milliseconds(100))) {
                cout << "BLOCKING ENQUEUE WITH TIMEOUT = " << item << "\n";
            } else {
                cout << "QUEUE FULL (TIMEOUT)\n";
            }
        } else if (type == 3) {
            // Size Query (blocking)
            cout << "BLOCKING SIZE QUERY = " << q.size_blocking() << "\n";
        } else if (type == 4) {
            // Try Size Query (non-blocking)
            int size = q.try_size();
            if (size != -1) {
                cout << "NON BLOCKING SIZE QUERY = " << size << "\n";
            } else {
                cout << "COULD NOT GET SIZE DUE TO CONTENTION\n";
            }
        } else if (type == 5) {
            // Size Query with Timeout (blocking)
            cout << "BLOCKING SIZE QUERY WITH TIMEOUT = " << q.size_blocking_with_timeout(chrono::milliseconds(100)) << "\n";
        } else if (type == 6) {
            // Dequeue (blocking)
            if (q.pop_blocking()) {
                cout << "BLOCKING POP\n";
            } else {
                cout << "QUEUE EMPTY (BLOCKING POP)\n";
            }
        } else if (type == 7) {
            // Try Dequeue (non-blocking)
            if (q.try_pop()) {
                cout << "NON BLOCKING POP\n";
            } else {
                cout << "COULD NOT POP DUE TO EMPTY QUEUE\n";
            }
        } else if (type == 8) {
            // Dequeue with Timeout (blocking)
            if (q.pop_blocking_with_timeout(chrono::milliseconds(100))) {
                cout << "BLOCKING POP WITH TIMEOUT\n";
            } else {
                cout << "QUEUE EMPTY (TIMEOUT POP)\n";
            }
        } else {
            // Peek (blocking)
            int front_item;
            if (q.peek_blocking(front_item)) {
                cout << "BLOCKING PEEK FRONT = " << front_item << "\n";
            } else {
                cout << "QUEUE EMPTY (BLOCKING PEEK FRONT)\n";
            }
        }
    }
}

void stress_test() {
    thread t[NUM_THREADS];
    thread_safe_queue<int> q;  

    for (int i = 0; i < NUM_THREADS; i++) {
        t[i] = move(thread(workers, ref(q)));  
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        t[i].join();
    }
}

int main(int argc, char* argv[]) {
    srand(time(0)); 
    stress_test();   
    return 0;
}
