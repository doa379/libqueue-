#ifndef TPOOL_HPP
#define TPOOL_HPP

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

class Tpool
{
  thread *th;
  void worker(void);
  queue<function<void()>> q;
  mutex q_mutex;
  condition_variable cond_var;
  bool quit;
  
protected:
  function<void()> job;

public:
  Tpool(void);
  ~Tpool(void);
  void tpool_exit(void);
  void add_job(function<void()>);
  size_t count(void);
};

#endif
