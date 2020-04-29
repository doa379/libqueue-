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
  while(!q.empty()) q.pop();
  cv.notify_one();
}

void Tpool::worker(void)
{  
  std::function<void()> curr_job;

  while (1)
  {
    {
      std::unique_lock<std::mutex> lock(q_mtx);

      while (q.empty() && !quit) // Sleeping
        cv.wait(lock);

      if (quit)
        break;

      curr_job = q.front();
    }

    curr_job();

    {
      std::unique_lock<std::mutex> lock(q_mtx);
      q.pop();
    }
  }
}

void Tpool::add_job(std::function<void()> job)
{
  std::unique_lock<std::mutex> lock(q_mtx);

  if (!quit)
  {
    q.push(job);
    cv.notify_one();
  }
}

size_t Tpool::job_count(void)
{
  size_t count;
  std::lock_guard<std::mutex> lock(q_mtx);
  count = q.size();
  return count;
}

void Tpool::suspend(void)
{
  std::lock_guard<std::mutex> lock(q_mtx);
  quit = 1;
  cv.notify_one();
}
