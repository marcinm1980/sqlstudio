
#include <stdio.h>
#include <stdexcept>

#include "base/threaded_timer.h"
#include "base/log.h"
//#include "base/threading.h"

// 30 fps should ensure smooth animations. Higher values are better, but put higher load on a system.
constexpr int BASE_FREQUENCY = 30;
// Define the maximum number of worker threads. If they are used up tasks have to wait.
constexpr int workerThreadCount = 2;

DEFAULT_LOG_DOMAIN(DOMAIN_BASE)

struct ThreadTImerFunctor : PoolFunctor {
  ThreadTImerFunctor(ThreadedTimer *timer, TimerTask *task) : _timer(timer), _task(task) {}
  ThreadedTimer *_timer;
  TimerTask *_task;

  void operator()() override {
   // ThreadedTimer::pool_function(*_timer, _task);
  }
};

//--------------------------------------------------------------------------------------------------

static ThreadedTimer *_timer = nullptr;
static std::mutex _timer_static_mutex;
/**
 * Returns the singleton instance of the timer.
 */
ThreadedTimer *ThreadedTimer::get() {
  std::lock_guard<std::mutex> guard(_timer_static_mutex);
  if (_timer == nullptr) {
    _timer = new ThreadedTimer(BASE_FREQUENCY);
  }
  return _timer;
}

//--------------------------------------------------------------------------------------------------

/**
 * Called from the main framework when the application goes down. So we can stop all threads
 * gracefully.
 */
void ThreadedTimer::stop() {
  delete _timer;
  _timer = NULL;
}

//--------------------------------------------------------------------------------------------------

/**
 * Used to add a new task (either a re-occuring or a one-shot task) to the timer's task list.
 *
 * @param unit Specifies in which unit the given value is. One can specify time spans and other units.
 * @param value A value which must be interpreted in the given unit. It can be:
 *              - A frequency (given in Hz).
 *              - A time span (given in seconds).
 * @param single_shot True, if this event must be triggered only once.
 * @param callback_ What to call when a timer event fires.
 * @result The id of the new task (can be used in the callback) or -1 if the task could not be added.
 */
int ThreadedTimer::add_task(TimerUnit unit, double value, bool single_shot, TimerFunction callback) {
  TimerTask task = {0, 0.0, 0.0, callback, false, single_shot, false};

  if (value <= 0)
    throw std::logic_error("The given timer value is invalid.");

  switch (unit) {
    case TimerFrequency:
      // The given value is a frequency. It must not be higher than our base frequency.
      // Note: giving a one-shot timer with a frequency doesn't make much sense, but we
      //       support this nonetheless.
      if (value > BASE_FREQUENCY)
        throw std::logic_error("The given task frequency is higher than the base frequency.");
      task.wait_time = 1 / value;
      break;
    case TimerTimeSpan:
      // The given value is a time span given in seconds.
      // It must not be lower than the minimal time span we support.
      if (value < 1.0 / BASE_FREQUENCY)
        throw std::logic_error("The given task time span is smaller than the smallest supported value.");
      task.wait_time = value;
      break;
  }
  if (task.wait_time > 0) {
    ThreadedTimer *timer = ThreadedTimer::get();
    std::lock_guard<std::mutex> guard(timer->_timer_lock);

    // in theory, it is possible to wrap around to 0 again.  Not a very likely scenario, but better safe than sorry
    if (timer->_next_id == 0) // 0 is special, skip it over
      timer->_next_id++;

    // We have the lock acquired so it is save to increment the id counter.
    task.task_id = timer->_next_id++;
    timer->_tasks.push_back(task);

    return task.task_id;
  }
  return -1;
}

//--------------------------------------------------------------------------------------------------

/**
 * Removes the given task from the task list by setting its stop flag. If the task is running
 * currently it can finish as usual. It is then removed on the next run of the scheduler.
 *
 * @param task_id The id of the task to remove. If it does not exist nothing happens.
 */
bool ThreadedTimer::remove_task(int task_id) {
  ThreadedTimer *timer = ThreadedTimer::get();
  return timer->remove(task_id);
}

//--------------------------------------------------------------------------------------------------

ThreadedTimer::ThreadedTimer(int base_frequency) : _pool(std::make_unique<ThreadPool>(workerThreadCount)),
  _terminate(false), _next_id(1) {
  // Wait time in microseconds.
  _wait_time = 1000 * 1000 / base_frequency;
  _thread = std::thread(start, std::ref(*this));
  //  #include <thread>
  //  #include <functional>
  //  #include <vector>

    // ...

    //std::vector<std::thread> _pool;

    // ...

    /*_pool.reserve(WORKER_THREAD_COUNT);
    for (int i = 0; i < WORKER_THREAD_COUNT; ++i) {
        _pool.emplace_back([this]() {
          pool_function(std::placeholders::_1, std::placeholders::_2);
        });
    }*/
}

//--------------------------------------------------------------------------------------------------

/**
 * Shuts down the timer and does not return until currently running threads have terminated.
 */
ThreadedTimer::~ThreadedTimer() {
  // Free the thread pool but wait until tasks, which are currently executing have finished.
  // Pending tasks are discarded.
  logDebug2("Threaded timer shutdown...\n");
  // Don't lock the mutex or we might deadlock here if the mutex is currently held by the work loop.
  _terminate = true;
  // Wait for the timer thread to terminate.
  _thread.join();

  logDebug2("Threaded timer shutdown done\n");
}

//--------------------------------------------------------------------------------------------------

/**
 * Main entry point for the timer thread.
 */
void ThreadedTimer::start(ThreadedTimer& data) {
  ThreadedTimer &thread = static_cast<ThreadedTimer&>(data);
  thread.main_loop();
}

//--------------------------------------------------------------------------------------------------

/**
 * Entry point for all pool (worker) threads.
 */
void ThreadedTimer::pool_function(ThreadedTimer& thread) {
  //ThreadedTimer *timer = static_cast<ThreadedTimer *>(user_data);
  //TimerTask *task = static_cast<TimerTask *>(data);

  //try {
  //  bool do_stop = task->callback(task->task_id);
  //  std::lock_guard<std::mutex> guard(timer->_timer_lock);
  //  task->stop = do_stop || task->single_shot;
  //  task->scheduled = false;
  //} catch (std::exception &e) {
  //  // In the case of an exception we remove the task silently.
  //  std::lock_guard<std::mutex> guard(timer->_timer_lock);
  //  task->stop = true;
  //  task->scheduled = false;
  //  logWarning("Threaded timer: exception in pool function: %s\n", e.what());
  //} catch (...) {
  //  // Most exceptions should be caught by the part above. Just to be on the safe side
  //  // do this extra branch.
  //  std::lock_guard<std::mutex> guard(timer->_timer_lock);
  //  task->stop = true;
  //  task->scheduled = false;
  //  logWarning("Threaded timer: unknown exception in pool function\n");
  //}

}

//--------------------------------------------------------------------------------------------------

// Helper predicate for removing finished tasks.
class IsStopped : public std::function<TimerTask (bool)> {
public:
  bool operator()(TimerTask &task) {
    return task.stop;
  }
};

//--------------------------------------------------------------------------------------------------

void ThreadedTimer::main_loop() {
  // Provides a high-quality clock which is used to compute execution times of tasks.
  auto start_time = std::chrono::steady_clock::now();

  while (!_terminate) {
    // This sleep call forms our base frequency.
    std::this_thread::sleep_for(std::chrono::microseconds(_wait_time));
    if (_terminate)
      break;

    // 1. Compute next execution time for new tasks.
    {
      std::lock_guard<std::mutex> guard(_timer_lock);
      for (auto& task : _tasks) {
        if (task.next_time == 0) {
          auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count();
          task.next_time = elapsed + task.wait_time;
        }
      }
    }

    // 2. Execute all tasks which are due now.
    // Processing of the task entries should be very fast here. No need to make a copy of them.
    {
      std::lock_guard<std::mutex> guard(_timer_lock);
      auto current_time = std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count();

      for (auto& task : _tasks) {
        if (_terminate)
          break;

        if (!task.scheduled && task.next_time <= current_time && !task.stop) {
          // Schedule the task for execution
          task.scheduled = true;
          task.next_time += task.wait_time;

          // Push the task to the thread pool
          ThreadTImerFunctor functor(this, &task);
          _pool->enqueue(std::move(functor));
        }
      }
    }

    // 3. Remove stopped task.
    {
      std::lock_guard<std::mutex> guard(_timer_lock);
      _tasks.remove_if([](const TimerTask& task) { return task.stop; });
    }
  }
}

//--------------------------------------------------------------------------------------------------

bool ThreadedTimer::remove(int task_id) {
  std::lock_guard<std::mutex> guard(_timer_lock);
  for (std::list<TimerTask>::iterator iterator = _tasks.begin(); iterator != _tasks.end(); iterator++) {
    if (iterator->task_id == task_id) {
      iterator->stop = true;
      TimerTask &task = *iterator;
      return _pool->remove_task(task.task_id);
    }
  }
  return true;
}

//--------------------------------------------------------------------------------------------------
