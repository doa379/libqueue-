#ifndef TPOOL_HPP
#define TPOOL_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class Tpool
{
  std::thread *th;
  void worker(void);
  std::queue<std::function<void()>> q;
  std::mutex q_mutex;
  std::condition_variable cond_var;
  bool quit;

public:
  Tpool(void);
  ~Tpool(void);
  void clear_jobs(void);
  void add_job(std::function<void()>);
  size_t job_count(void);
};

#endif
