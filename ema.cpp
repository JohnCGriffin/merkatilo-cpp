
#include "merkatilo.hpp"

namespace merkatilo {

/**
@brief exponential moving average
*
* Using N > 1, a fraction is calculated FRAC = (2 / (N+1)).  That weight
* is given to a current value with the remainder (1-FRAC) given to the 
* preceding date's calculation.  Observations with no immediately preceding
* observation simply pass through the current value.
*/

  series_ptr ema(series_ptr s, unsigned N)
  {
    if(N < 2){
      throw std::runtime_error("ema require N > 1");
    }
    double new_fraction = 2.0 / (N+1.0);
    double old_fraction = 1.0 - new_fraction;

    series_builder builder;
    value_type prev = default_value();

    auto obs = s->observations_by_date(current_dates::active());

    for (auto ob : *obs){
      auto dt = ob.dt;
      auto val = ob.val;
      if(valid(val) && valid(prev)){
	prev = new_fraction * val + old_fraction * prev;
      } else {
	prev = val;
      }
      if(valid(prev)){
	builder.insert({dt,prev});
      }
    }
    return builder.construct();
  }
  
}
