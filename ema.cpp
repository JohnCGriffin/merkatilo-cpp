
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr ema(series_ptr s, unsigned N)
  {
    double new_fraction = 2.0 / (N+1.0);
    double old_fraction = 1.0 - new_fraction;

    series_builder builder;
    opt_double prev;

    auto obs = s->observations_by_date(current_dates::active());

    for (auto ob : *obs){
      auto dt = ob.dt;
      auto val = ob.val;
      if(valid(val) && valid(prev)){
	prev = new_fraction * valueof(val) + old_fraction * valueof(prev);
      } else {
	prev = val;
      }
      if(valid(prev)){
	builder.insert({dt, valueof(prev)});
      }
    }
    return builder.construct();
  }
  
}
