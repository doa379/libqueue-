/*
  $ g++ test_tpool.cpp -L $PWD -l queue++ -Wl,-rpath,$PWD -o test_tpool && ./test_tpool
*/

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "tpool.hpp"

void func_cb(int v)
{
  std::cout << v << std::endl;
  sleep(5);
  std::cout << "Job exit" << std::endl;
}

int main()
{
  Tpool tpool;
  srand(0);

  for (unsigned i = 0; i < 5; i++)
    {
      int v = rand() % 100;
      tpool.add_job(std::bind(func_cb, v));
      std::cout << "Data " << v << "job added" << std::endl;
    }

  while (tpool.count())
    {
      std::cout << tpool.count() << " jobs remaining" << std::endl;
      sleep(1);
    }

  return 0;
}
