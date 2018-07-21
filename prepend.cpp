
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr prepend (series_ptr base, series_ptr surrogate,dateset_ptr dates)
  {
    const auto common_ob = first_ob(base + surrogate);
    const double ratio = base->at(common_ob.dt) / surrogate->at(common_ob.dt);

    series_builder builder;
    auto base_p = base.get();
    auto surrogate_p = surrogate.get();

    for (const auto dt : *dates){
      auto val = base_p->at(dt);
      if(!valid(val) && dt < common_ob.dt){
	val = surrogate_p->at(dt);
	if(valid(val)){
	  val *= ratio;
	}
      }
      if(valid(val)){
	builder.insert(observation{dt,val});
      }
    }

    return builder.construct();
  }

}

