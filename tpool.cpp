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

void Tpool::worker(void)
{  
  while (!quit)
  {
    {
      std::unique_lock<std::mutex> lock(q_mutex);

      while (q.empty() && !quit) // Sleeping
        cond_var.wait(lock);

      if (q.empty())
        continue;

      job = q.front();
      q.pop();
    }

    job();
  }
}

void Tpool::add_job(std::function<void()> job)
{
  {
    std::unique_lock<std::mutex> lock(q_mutex);
    q.push(job);
  }

  cond_var.notify_one();
}

size_t Tpool::job_count(void)
{
  return q.size();
}
