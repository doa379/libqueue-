/*
  $ g++ test_tpool.cpp -L $PWD -l queue++ -Wl,-rpath,$PWD -o test_tpool && ./test_tpool
*/

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "tpool.hpp"

void func_cb(const int *v)
{
  std::cout << *v << std::endl;
  sleep(5);
  std::cout << "Job exit" << std::endl;
}

int main()
{
  Tpool tpool;
  srand(0);
  std::vector<int> V;

  for (unsigned i = 0; i < 5; i++)
  {
    int v = rand() % 100;
    V.emplace_back(std::move(v));
  }

  for (std::vector<int>::iterator v = V.begin(); v < V.end(); v++)
  {
    tpool.add_job([v]() { func_cb(&*v); });
    std::cout << "Data " << *v << "job added" << std::endl;
  }

  while (tpool.job_count())
  {
    std::cout << tpool.job_count() << " jobs remaining" << std::endl;
    sleep(1);
  }

  return 0;
}
