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
  std::atomic<bool> m_forceExit { false };  
  public:
  void push(T const &val)
  {
    m_forceExit.store(false);
    std::unique_lock<std::mutex> lock(q_mtx);
    q.push(val);
    lock.unlock();
    cv.notify_one();
  }
  bool isEmpty() const
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
  bool wpop(T &val)
  {
    std::unique_lock<std::mutex> lock(q_mtx);
    cv.wait(lock, 
      [&]() -> bool { return !q.empty() || m_forceExit.load(); } );
    if (m_forceExit.load()) return false;
    val = q.front();
    q.pop();
    return true;
  }
  bool wtpop(T &val, unsigned long milliseconds = DEFAULT_WAITTIME_MS)
  {
    std::unique_lock<std::mutex> lock(q_mtx);
    cv.wait_for(lock, std::chrono::milliseconds(milliseconds), 
      [&]() -> bool { return !q.empty() || m_forceExit.load(); } );
    if (m_forceExit.load()) return false;
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
    m_forceExit.store(true);
    std::unique_lock<std::mutex> lock(q_mtx);
    while (!q.empty())
    {
      delete q.front();
      q.pop();
    }
  }
  bool isExit() const
  {
    return m_forceExit.load();
  }
};
