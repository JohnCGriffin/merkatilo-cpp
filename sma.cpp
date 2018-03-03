
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr sma(series_ptr s, unsigned N)
  {
    auto sp = s.get();
    double total = 0;
    unsigned count = 0;
    double cycle[N];

    series_builder builder;

    for (auto dt : *current_dates::active()){
      
      auto val = sp->at(dt);
      
      if(!val){
	total = count = 0;
	
      } else {
	double v = val.value();
	int slot = count % N;
	count++;
	total += v;
	if(count > N){
	  total -= cycle[slot];
	}
	cycle[slot] = v;
	builder.insert({dt,(total/N)});
      }
    }
    return builder.construct();
  }
  
}
