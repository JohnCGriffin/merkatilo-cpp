
#include "merkatilo.hpp"

namespace merkatilo {
  
  std::pair<observation,observation> min_max_obs (series_ptr sp) {

    auto s = sp.get();
    
    auto min_ob = ([&]() -> observation {
      for (auto dt : *current_dates::active()){
	auto val = s->at(dt);
	if(valid(val)){
	  return { dt, val };
	}
      }
      throw std::runtime_error("no observations");
      })();
    
    auto max_ob = min_ob;

    for(auto dt : *current_dates::active()){
      auto val = s->at(dt);
      if(valid(val)){
	if(val < min_ob.val){
	  min_ob = { dt, val };
	}
	if(val >max_ob.val){
	  max_ob = { dt, val };
	}
      }
    }

    return { min_ob, max_ob };
    
  }

}
