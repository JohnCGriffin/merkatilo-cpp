
#include "merkatilo.hpp"

namespace merkatilo {

/**
 *@brief first observation in a series
 */

  observation first_ob(series_ptr sp){
    auto s = sp.get();
    for(auto dt : *current_dates::active()){
      auto val = s->at(dt);
      if(valid(val)){
	return { dt, val };
      }
    }
    throw std::runtime_error("no observations");
  }

/**
 *@brief last observation in a series
 */

  observation last_ob(series_ptr sp){
    auto s = sp.get();
    auto dts = current_dates::active();
    for(auto it = dts->rbegin(); it != dts->rend(); ++it){
      auto dt = *it;
      auto val = s->at(dt);
      if(valid(val)){
	return { dt, val };
      }
    }
    throw std::runtime_error("no observations");
  }

}
