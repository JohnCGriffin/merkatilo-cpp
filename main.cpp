
#include <iostream>
#include "merkatilo.hpp"
#include <chrono>
#include <functional>

void timeit(std::string title, std::function<void(void)> f)
{
  auto start = std::chrono::system_clock::now();
  for(int i=0; i<100; i++){
    f();
  }
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double,std::micro> d = (end - start);
  std::cout << title << " " << (d.count()/100) << " microseconds" << std::endl;
}

int main()
{
  using namespace merkatilo;

  auto IBM = lo("IBM");

  {
    current_dates active(IBM);
    timeit("drawdown", [&](){ drawdown(IBM); });
    timeit("momentum", [&](){ mo(IBM,20); });
    timeit("ema", [&](){ ema(IBM,20); });
    timeit("sma", [&](){ sma(IBM,20); });
    auto mo5 = mo(IBM,5);
    timeit("conviction", [&](){ conviction(mo5,4); });
    timeit("to_signals", [&](){ to_signals(mo5); });
    auto sm200 = sma(IBM,200);
    timeit("cross", [&](){ cross(sm200,IBM); });
    timeit("investment_performance", [&](){ investment_performance(IBM); });
    
  }

}
