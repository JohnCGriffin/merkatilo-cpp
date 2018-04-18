

#include <iostream>
#include "merkatilo.hpp"

namespace merkatilo {

  /// For N > 1, calculate the average of the last N observations at each
  /// date.  Any missing value with the last N periods results in NaN output.

  series_ptr ma(series_ptr s, unsigned N, dateset_ptr dates)
  {
    if(N < 2){
      throw std::invalid_argument("ma require N > 1");
    }
    double total = 0;
    unsigned count = 0;
    std::vector<double> cycle(N);

    series_builder builder;

    const auto obs = s->observations_by_date(dates);

    for (const auto& ob : *obs){

      const auto dt = ob.dt;
      const auto val = ob.val;
      
      if(!valid(val)){
	total = count = 0;
	
      } else {
	const int slot = count % N;
	count++;
	total += val;
	if(count > N){
	  total -= cycle[slot];
	}
	cycle[slot] = val;
	if(count >= N){
	  builder.insert({dt,(total/N)});
	}
      }
    }
    return builder.construct();
  }
  
}
