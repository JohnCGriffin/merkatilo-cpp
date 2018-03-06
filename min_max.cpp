
#include "merkatilo.hpp"

namespace merkatilo {
  
  obpair min_max_obs (series_ptr sp) {

    auto s = sp.get();
    auto min_ob = first_ob(sp);
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
