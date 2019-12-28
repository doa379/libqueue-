#include "tpool.hpp"

Tpool::Tpool(void)
{
  quit = 0;
  th = new std::thread(&Tpool::worker, this);
}

Tpool::~Tpool(void)
{
  tpool_exit();

  if (th)
    {
      th->join();
      delete th;
    }
}

void Tpool::tpool_exit(void)
{
  quit = 1;

  {
    std::lock_guard<std::mutex> lock(q_mutex);
    cond_var.notify_one();
  }
}

void Tpool::worker(void)
{  
  while (!quit)
    {
      {
	std::unique_lock<std::mutex> lock(q_mutex);

	while (q.empty() && !quit)
	  {
	    // Sleeping
	    cond_var.wait(lock);
	  }

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

size_t Tpool::count(void)
{
  return q.size();
}
