
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr ema(series_ptr s, unsigned N)
  {
    double new_fraction = 2.0 / (N+1.0);
    double old_fraction = 1.0 - new_fraction;

    series_builder builder;
    opt_double prev;

    const auto dates = current_dates::active();
    const auto dates_p = dates.get();
    const size_t len = dates->size();
    auto vals = s->values_of_dates(dates);
    auto vals_p = vals.get();

    for(size_t i=0; i<len; i++){
      auto dt = dates_p->at(i);
      auto val = vals_p->at(i);
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
