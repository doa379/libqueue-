#include "tpool.hpp"

Tpool::Tpool(void)
{
  quit = 0;
  th = new thread(&Tpool::worker, this);
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
    lock_guard<mutex> lock(q_mutex);
    cond_var.notify_one();
  }
}

void Tpool::worker(void)
{  
  while (!quit)
    {
      {
	unique_lock<mutex> lock(q_mutex);

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

void Tpool::add_job(function<void()> job)
{
  {
    unique_lock<mutex> lock(q_mutex);
    q.push(job);
  }

  cond_var.notify_one();
}

size_t Tpool::count(void)
{
  return q.size();
}
