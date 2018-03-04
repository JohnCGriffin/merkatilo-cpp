
#include "merkatilo.hpp"

namespace merkatilo {

  value_type sign(value_type x)
  {
    return (x < 0) ? -1.0 : 1.0;
  }

  series_ptr to_signals(series_ptr sp)
  {
    auto s = sp.get();
    auto prev = 0.0;
    series_builder builder;
    
    for(auto dt : *current_dates::active()){
      auto val = s->at(dt);
      if(valid(val)){
	auto sig = sign(val);
	if(sig != prev){
	  prev = sig;
	  builder.insert({dt, sig});
	}
      }
    }
    return builder.construct();
  }

}
