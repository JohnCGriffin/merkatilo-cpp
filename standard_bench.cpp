
#include "merkatilo.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace merkatilo;
using namespace std::chrono;

static series_ptr test_lo (std::string name){
  return lo("/tmp/merkatilo-test-data/" + name + ".txt");
}


void measure(std::string name, std::function<series_ptr(void)> thunk)
{
  const int ITERATIONS = 200;
  auto start = high_resolution_clock::now();
  for(int i=0; i<ITERATIONS; i++){
    auto s = thunk();
    auto sp = s.get();
    dateset_ptr dates=current_dates::active();
    for (const auto dt : *dates){
      sp->at(dt);
    }
  }
  auto end = high_resolution_clock::now();
  duration<double> elapsed = (end-start);
  auto speed = (1.0 * ITERATIONS) / elapsed.count();
  std::cout << std::left << std::setw(12) << name << "\t" << int(speed) << std::endl;
}


int main(int argc, char** argv)
{
  auto BENCHMARK = test_lo("benchmark");
  current_dates active(BENCHMARK);
  auto EMA = ema(BENCHMARK,10);
  auto SIGS = cross(EMA,BENCHMARK);
  measure("conviction", [&](){ return conviction(SIGS,2); });
  measure("cross", [&](){ return cross(EMA,BENCHMARK); });
  measure("dd", [&](){ series_drawdown(BENCHMARK); return constant(2.2); });
  measure("ema", [&]() { return ema(BENCHMARK,10); });
  measure("equity", [&]() { return equity_line(BENCHMARK,SIGS); });
  measure("fudge", [&]() { return fudge(BENCHMARK); });
  measure("ma", [&](){ return ma(BENCHMARK,10); });
}
