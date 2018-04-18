
#include "merkatilo.hpp"


namespace merkatilo {

  series_ptr unrepeated (series_ptr sp, dateset_ptr dates) {
    const auto s = sp.get();
    series_builder builder;
    double prev = default_value();
    for (const auto dt : *dates){
      const auto val = s->at(dt);
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
