
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr repeated (series_ptr sp, bool repeat_last){

    auto s = sp.get();
    observations obs;
    observation last_ob { 0, default_value() };

    for (auto dt : *current_dates::active()){

      auto val = s->at(dt);

      if(valid(val)){
	last_ob = { dt, val };
	obs.push_back(last_ob);
	
      } else if (last_ob.dt) {
	obs.push_back({ dt, last_ob.val });
	
      }
      
    }

    series_builder builder;

    for (auto ob : obs){
      if(repeat_last || ob.dt <= last_ob.dt){
	builder.insert(ob);
      }
    }

    return builder.construct();
  }

}
