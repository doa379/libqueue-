// $ g++ test_queue.cpp -l pthread -o test_queue && ./test_queue 
#include "queue.h"
#include <iostream>
#include <thread>
#include <unistd.h>

void func_th(Queue<unsigned> *q)
{
  for (unsigned i = 0; i < 10; i++)
    q->push(i);
}

int main()
{
  Queue<unsigned> q;
  std::thread th(func_th, &q);
  sleep(1);
  while (!q.is_empty())
  {
    unsigned v;
    if (q.pop(v))
      std::cout << v << std::endl;
  }
  th.join();
  return 0;
}
