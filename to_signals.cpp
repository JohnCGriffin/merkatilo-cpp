
#include "merkatilo.hpp"

namespace merkatilo {

  double sign(double x)
  {
    return (x < 0) ? -1.0 : 1.0;
  }

  /// Any value less then zero is assigned -1 and the remainder are
  /// assigned 1.  Repeated output values are suppressed.  A common use is
  /// to convert the output of a 'mo' to a proper signal.  Calling
  /// to_signals on the output of to_signals is an identity.
  
  series_ptr to_signals(series_ptr sp, dateset_ptr dates)
  {
    auto s = sp.get();
    auto prev = 0.0;
    series_builder builder;
    
    for(auto dt : *dates){
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
