#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <vector>

struct PoolFunctor {
  PoolFunctor() = default;
  PoolFunctor(PoolFunctor&&) = default;
  PoolFunctor(const PoolFunctor&) = default;
  PoolFunctor& operator=(PoolFunctor&&) = default;
  virtual void operator()() {
    return;
  }
  virtual ~PoolFunctor() {}
  int id_;
};

class ThreadPool {
public:
  ThreadPool(int num_threads);
  ~ThreadPool();

  void enqueue(PoolFunctor task);
  bool remove_task(int task_id);

private:
  std::vector<std::thread> workers_;
  std::queue<PoolFunctor> tasks_;

  std::mutex queue_mutex_;
  std::condition_variable condition_;
  bool stop_;

};