
#include <iostream>
#include "merkatilo.hpp"

int main()
{
  using namespace merkatilo;
  auto tdy = jdate(2018,2,27);
  for (int i=0; i<10; i++){
    std::cout << (tdy + i) << std::endl;
  }

  std::cout << "today is " << today() << std::endl;

  std::cout << "today year " << today().year() << std::endl;
  std::cout << "today month " << today().month() << std::endl;
  std::cout << "today day " << today().day() << std::endl;

  auto IBM = lo("IBM");

  for(auto dt = today()-20; dt<today(); dt = dt+1){
    auto val = IBM->at(dt);
    if(val){
      std::cout << dt << " " << (*val) << "\n";
    }
  }

  auto ds = dateset_builder(IBM).construct();

  for(auto dt : ds){
    std::cout << dt << " " << IBM->at(dt).value() << "\n";
  }

  current_dates active(IBM);
  dump({IBM, ema(IBM,20), sma(IBM,20) });

  for(int i=0; i<1000; i++){
    sma(IBM,i+2);
    ema(IBM,i+2);
  }
  
}
