#pragma once
#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <atomic>
#define DEFAULT_WAITTIME_MS 1000

template<typename T>
class Queue
{
  std::queue<T> q;       
  std::mutex q_mtx;                 
  std::condition_variable cv;       
  std::atomic<bool> quit { false };  
  public:
  void push(T const &val)
  {
    quit.store(false);
    std::unique_lock<std::mutex> lock(q_mtx);
    q.push(val);
    lock.unlock();
    cv.notify_one();
  }
  bool is_empty()
  {
    std::unique_lock<std::mutex> lock(q_mtx);
    return q.empty();
  }
  bool pop(T &val)
  {
    std::unique_lock<std::mutex> lock(q_mtx);
    if (q.empty())
      return false;
    val = q.front();
    q.pop();
    return true;
  }
  bool waitpop(T &val)
  {
    std::unique_lock<std::mutex> lock(q_mtx);
    cv.wait(lock, 
      [&]() -> bool { return !q.empty() || quit.load(); } );
    if (quit.load()) return false;
    val = q.front();
    q.pop();
    return true;
  }
  bool waitpop_time(T &val, unsigned long milliseconds = DEFAULT_WAITTIME_MS)
  {
    std::unique_lock<std::mutex> lock(q_mtx);
    cv.wait_for(lock, std::chrono::milliseconds(milliseconds), 
      [&]() -> bool { return !q.empty() || quit.load(); } );
    if (quit.load()) return false;
    if (q.empty()) return false;
    val = q.front();
    q.pop();
    return true;
  }
  unsigned size()
  {   
    std::unique_lock<std::mutex> lock(q_mtx);
    return static_cast<unsigned>(q.size());
  }
  void clear()
  { 
    quit.store(true);
    std::unique_lock<std::mutex> lock(q_mtx);
    while (!q.empty())
    {
      delete q.front();
      q.pop();
    }
  }
  bool is_exit() const
  {
    return quit.load();
  }
};
