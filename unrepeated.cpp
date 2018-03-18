
#include "merkatilo.hpp"


namespace merkatilo {

/**
@brief remove contiguous repeated values.
*/

  series_ptr unrepeated (series_ptr sp, dateset_ptr dates) {
    auto s = sp.get();
    series_builder builder;
    double prev = default_value();
    for (auto dt : *dates){
      auto val = s->at(dt);
      if(val == prev){
	;
      } else if(valid(val)){
	builder.insert({dt, val});
      }
      prev = val;
    }
    return builder.construct();
  }

}
