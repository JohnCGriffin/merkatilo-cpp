
#include <iostream>
#include "merkatilo.hpp"

int main()
{
  using namespace merkatilo;
  auto tdy = ymd_to_jdate(2018,2,27);
  for (int i=0; i<10; i++){
    std::cout << (tdy + i) << std::endl;
  }

  std::cout << "today is " << today() << std::endl;

  std::cout << "today year " << year(today()) << std::endl;
  std::cout << "today month " << month(today()) << std::endl;
  std::cout << "today day " << day(today()) << std::endl;

  auto IBM = lo("IBM");

  for(auto dt = today()-20; dt<today(); dt = dt+1){
    auto val = IBM->at(dt);
    if(valid(val)){
      std::cout << dt << " " << valueof(val) << "\n";
    }
  }

  auto ds = dateset_builder(IBM).construct();

  for(auto dt : ds){
    std::cout << dt << " " << valueof(IBM->at(dt)) << "\n";
  }

  {
    current_dates active(IBM);
    dump({IBM, ema(IBM,20), sma(IBM,20) });
  }
  
  {
    dateset_builder builder(today()-30,today());
    auto ds = std::make_shared<dateset>(builder.construct());
    current_dates recent(ds);
    dump({IBM, ema(IBM,4), sma(IBM,4) });
    sma(IBM,11);
  }

  {
    current_dates active(IBM);
    for(int i=20; i<1000; i++){
      sma(IBM,i+2);
      ema(IBM,i+2);
    }
  }
  
}
