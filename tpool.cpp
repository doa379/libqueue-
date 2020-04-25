#include "tpool.hpp"

Tpool::Tpool(void)
{
  quit = 0;
  th = new std::thread(&Tpool::worker, this);
}

Tpool::~Tpool(void)
{
  {
    std::lock_guard<std::mutex> lock(q_mutex);
    quit = 1;
    while(!q.empty()) q.pop();
    cond_var.notify_one();
  }

  th->join();
  delete th;
}

void Tpool::clear_jobs(void)
{
  std::lock_guard<std::mutex> lock(q_mutex);
  while(!q.empty()) q.pop();
  cond_var.notify_one();
}

void Tpool::worker(void)
{  
  std::function<void()> curr_job;

  while (1)
  {
    {
      std::unique_lock<std::mutex> lock(q_mutex);

      while (q.empty() && !quit) // Sleeping
        cond_var.wait(lock);

      if (quit)
        break;

      else if (q.empty())
        continue;

      curr_job = q.front();
    }

    curr_job();

    {
      std::unique_lock<std::mutex> lock(q_mutex);
      q.pop();
    }
  }
}

void Tpool::add_job(std::function<void()> job)
{
  std::unique_lock<std::mutex> lock(q_mutex);
  q.push(job);
  cond_var.notify_one();
}

size_t Tpool::job_count(void)
{
  size_t count;
  std::lock_guard<std::mutex> lock(q_mutex);
  count = q.size();
  return count;
}
