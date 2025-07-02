// --- TaskScheduler.cpp ---
// A simple task scheduler that runs jobs in a background thread.
// It's meant to offload work from a primary (e.g., UI) thread.

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
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
            std::lock_guard<std::mutex> lock(m_mutex);
            m_shouldStop = true;
        }
        m_workerThread.join();
    }

    void submitTask(Task task) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(task);
        // A condition variable would be better here, but that's not the bug.
    }

    bool hasPendingTasks() const {
        // This method is intended to be called from other threads to check status.
        return !m_tasks.empty();
    }

private:
    void workerLoop() {
        while (true) {
            Task currentTask;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (m_shouldStop && m_tasks.empty()) {
                    return; // Exit condition
                }
                if (!m_tasks.empty()) {
                    currentTask = m_tasks.front();
                    m_tasks.pop();
                }
            } // Mutex is unlocked here

            if (currentTask) {
                currentTask(); // Execute task outside of lock
            }
            
            // Sleep to prevent busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    std::thread m_workerThread;
    std::queue<Task> m_tasks;
    std::mutex m_mutex;
    bool m_shouldStop;
};