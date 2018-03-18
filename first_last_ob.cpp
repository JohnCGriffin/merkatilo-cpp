
#include "merkatilo.hpp"

namespace merkatilo {

/**
 *@brief first observation in a series
 */

  observation first_ob(series_ptr sp, dateset_ptr dates){
    auto s = sp.get();
    for(auto dt : *dates){
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

  observation last_ob(series_ptr sp, dateset_ptr dates){
    auto s = sp.get();
    for(auto it = dates->rbegin(); it != dates->rend(); ++it){
      auto dt = *it;
      auto val = s->at(dt);
      if(valid(val)){
	return { dt, val };
      }
    }
    throw std::runtime_error("no observations");
  }

}
