
#include "merkatilo.hpp"

namespace merkatilo {

  /**
     @brief convert vector<observation> into a series.
  */
  series_ptr obs_to_series(observations_ptr obs){
    series_builder builder;
    for (const auto& ob : *obs){
      builder.insert(ob);
    }
    return builder.construct();
  }

  /**
     @brief convert series into vector<observation>
  */
  observations_ptr series_to_obs(series_ptr sp, dateset_ptr dates){
    auto s = sp.get();
    observations obs;
    for(auto dt : *dates){
      auto val = s->at(dt);
      if(valid(val)){
	obs.push_back({dt, val});
      }
    }
    return std::make_shared<observations>(obs);
  }


}
