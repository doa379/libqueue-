#include "tpool.hpp"

Tpool::Tpool(void)
{
  quit = 0;
  th = new std::thread(&Tpool::worker, this);
}

Tpool::~Tpool(void)
{
  suspend();
  th->join();
  delete th;
}

void Tpool::clear_jobs(void)
{
  std::lock_guard<std::mutex> lock(q_mtx);
  while (!q.empty()) q.pop();
  cv.notify_one();
}

void Tpool::worker(void)
{  
  std::function<void()> curr_job;

  while (1)
  {
    {
      std::unique_lock<std::mutex> lock(q_mtx);
      cv.wait(lock, [this](){ return !q.empty() || quit; });

      if (quit)
        return;

      curr_job = q.front();
    }

    curr_job();

    {
      std::lock_guard<std::mutex> lock(q_mtx);
      q.pop();
    }
  }
}

void Tpool::add_job(std::function<void()> job)
{
  if (quit)
    return;

  std::lock_guard<std::mutex> lock(q_mtx);
  q.push(job);
  cv.notify_one();
}

size_t Tpool::job_count(void)
{
  size_t q_size;

  {
    std::lock_guard<std::mutex> lock(q_mtx);
    q_size = q.size();
  }

  return q_size;
}

void Tpool::suspend(void)
{
  std::lock_guard<std::mutex> lock(q_mtx);
  quit = 1;
  cv.notify_one();
}
