
#include "merkatilo.hpp"

namespace merkatilo {

  /// Each output is (fraction * value) + ((1-fraction) * previous_result).
  
  series_ptr fractional(series_ptr s, double fraction, dateset_ptr dates)
  {
    if(fraction <= 0 || fraction > 1){
      throw std::invalid_argument("fractional require fraction 0 <= fraction < 1.");
    }
    
    double old_fraction = 1.0 - fraction;

    series_builder builder;
    double prev = default_value();

    auto obs = s->observations_by_date(dates);

    for (auto ob : *obs){
      auto dt = ob.dt;
      auto val = ob.val;
      if(valid(val) && valid(prev)){
	prev = fraction * val + old_fraction * prev;
      } else {
	prev = val;
      }
      if(valid(prev)){
	builder.insert({dt,prev});
      }
    }
    return builder.construct();
  }


  /// Using N > 1, a fraction is calculated FRAC = (2 / (N+1)).  That fraction
  /// is then supplied to 'fractional'.

  series_ptr ema(series_ptr s, unsigned N, dateset_ptr dates)
  {
    if(N < 2){
      throw std::invalid_argument("ema require N > 1");
    }
    return fractional(s, 2.0 / (N+1.0), dates);
  }
  
}
