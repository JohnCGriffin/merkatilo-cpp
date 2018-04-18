
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr obs_to_series(observations_ptr obs){
    series_builder builder;
    for (const auto& ob : *obs){
      builder.insert(ob);
    }
    return builder.construct();
  }

  observations_ptr series_to_obs(series_ptr sp, dateset_ptr dates){
    const auto s = sp.get();
    observations obs;
    for(const auto dt : *dates){
      const auto val = s->at(dt);
      if(valid(val)){
	obs.push_back({dt, val});
      }
    }
    return std::make_shared<observations>(obs);
  }


}
