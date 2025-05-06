#include "base/thread_pool.h"

// --------------------------------------------------------------------------------------------------

ThreadPool::ThreadPool(int num_threads) : stop_(false) {
  for (int i = 0; i < num_threads; ++i) {
    workers_.emplace_back([this] {
      while (true) {
        PoolFunctor task; {
          std::unique_lock<std::mutex> lock(this->queue_mutex_);
          this->condition_.wait(lock, [this] {
            return this->stop_ || !this->tasks_.empty();
          });
          if (this->stop_ && this->tasks_.empty()) {
            return;
          }
          task = std::move(this->tasks_.front());
          this->tasks_.pop();
        }
        task();
      }
    });
  }
}

// --------------------------------------------------------------------------------------------------

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    stop_ = true;
  }
  condition_.notify_all();
  for (std::thread &worker : workers_) {
    worker.join();
  }
}

// -------------------------------------------------------------------------------------------------- 

void ThreadPool::enqueue(PoolFunctor task) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    tasks_.emplace(std::move(task));
  }
  condition_.notify_one();
}

bool ThreadPool::remove_task(int task_id)
{
  std::unique_lock<std::mutex> lock(queue_mutex_);

  std::queue<PoolFunctor> new_tasks;
  bool found = false;
  while (!tasks_.empty()) {
    auto& task = tasks_.front();
    tasks_.pop();

    // Check if the task ID matches the one we want to remove
    if (task_id == task.id_) {
      found = true; // Task found, do not add it to new_tasks
    } else {
      new_tasks.push(task); // Keep the task in the new queue
    }
  }
  tasks_ = std::move(new_tasks);
  return found;
}

// -------------------------------------------------------------------------------  ------------------