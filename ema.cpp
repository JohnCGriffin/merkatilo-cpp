
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr ema(series_ptr s, unsigned N)
  {
    auto sp = s.get();
    double new_fraction = 2.0 / (N+1.0);
    double old_fraction = 1.0 - new_fraction;

    series_builder builder;
    std::optional<double> prev;

    for (auto dt : current_dates::active()){
      auto val = sp->at(dt);
      if(val && prev){
	prev = new_fraction * val.value() + old_fraction * prev.value();
      } else {
	prev = val;
      }
      if(prev){
	builder.insert({dt, prev.value()});
      }
    }
    return builder.construct();
  }
  
}
