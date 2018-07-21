
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
  const int ITERATIONS = 500;
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


int main()
{
  auto BENCHMARK = test_lo("benchmark");
  auto TEST_SERIES = test_lo("test-series");
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
  measure("math-add", [&](){ return BENCHMARK+BENCHMARK; });
  measure("min-max", [&](){ min_max_obs(BENCHMARK); return constant(1.3); });
  measure("mo", [&](){ return mo(BENCHMARK,10); });
  measure("mo-days", [&](){ return mo_days(BENCHMARK,10); });
  measure("prepend", [&](){ return prepend(TEST_SERIES, BENCHMARK); });
  measure("repeated", [&](){ return repeated(BENCHMARK); });
  measure("reversal", [&](){ return reversals(BENCHMARK,0.9,1.1); });
  measure("signal", [&](){ return to_signals(SIGS); });
  measure("unrepeated", [&](){ return unrepeated(BENCHMARK); });
  measure("volatility", [&](){ volatility(BENCHMARK); return constant(3.2); });
  measure("warp", [&](){ return warp(BENCHMARK,10); });

}
