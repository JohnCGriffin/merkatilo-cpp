
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr unrepeated (series_ptr sp) {
    auto s = sp.get();
    series_builder builder;
    double prev = default_value();
    for (auto dt : *current_dates::active()){
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
