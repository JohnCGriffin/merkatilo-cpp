

#include <iostream>
#include "merkatilo.hpp"

namespace merkatilo {

/**
@brief moving average
*
* For N > 1, calculate the average of the last N observations at each
* date.  Any missing value with the last N periods results in NaN output.
*/

  series_ptr sma(series_ptr s, unsigned N)
  {
    if(N < 2){
      throw std::runtime_error("sma require N > 1");
    }
    double total = 0;
    unsigned count = 0;
    double cycle[N];

    series_builder builder;

    auto obs = s->observations_by_date(current_dates::active());

    for (const auto& ob : *obs){

      auto dt = ob.dt;
      auto val = ob.val;
      
      if(!valid(val)){
	total = count = 0;
	
      } else {
	int slot = count % N;
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
