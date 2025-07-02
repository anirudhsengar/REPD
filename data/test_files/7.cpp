// --- TaskScheduler_Fixed.cpp ---
// A robust, thread-safe task scheduler that runs jobs in a background thread.
// It uses a condition variable to avoid busy-waiting and ensures all
// shared data access is properly synchronized.

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>

class TaskScheduler {
public:
    using Task = std::function<void()>;

    TaskScheduler() : m_shouldStop(false) {
        m_workerThread = std::thread(&TaskScheduler::workerLoop, this);
    }

    ~TaskScheduler() {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_shouldStop = true;
        }
        // Notify the worker thread to wake up and check the stop condition.
        m_condition.notify_one();
        m_workerThread.join();
    }

    void submitTask(Task task) {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_tasks.push(std::move(task));
        }
        // Notify the worker that a new task is available.
        m_condition.notify_one();
    }

    // This method is now thread-safe.
    bool hasPendingTasks() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return !m_tasks.empty();
    }

    // Prevent copying and assignment.
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

private:
    void workerLoop() {
        while (true) {
            Task currentTask;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                // The worker thread will sleep here until the condition is met.
                // The condition is: either stop is requested or there are tasks.
                m_condition.wait(lock, [this] { return m_shouldStop || !m_tasks.empty(); });

                // If woken up to stop and no tasks are left, exit.
                if (m_shouldStop && m_tasks.empty()) {
                    return;
                }
                
                // A task is guaranteed to be available here unless we are stopping.
                currentTask = std::move(m_tasks.front());
                m_tasks.pop();
            }

            // Execute task outside of the lock to allow other threads
            // to submit new tasks concurrently.
            currentTask();
        }
    }

    std::thread m_workerThread;
    std::queue<Task> m_tasks;
    mutable std::mutex m_mutex; // Mutable to allow locking in const methods like hasPendingTasks.
    std::condition_variable m_condition;
    bool m_shouldStop;
};