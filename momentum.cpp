
#include "merkatilo.hpp"

/**@file
 *@brief periodic ratio of (new/old)-1.  Thus, a 10% gain is 0.1 and a 2% loss
 * is -0.02.  
 */

namespace merkatilo {

  static value_type change (value_type val, value_type old_val){
    if(!valid(old_val) || !valid(val) || old_val == 0){
      return default_value();
    }
    return (val / old_val) - 1.0;
  }

  series_ptr mo (series_ptr sp, unsigned N) {

    if(N < 1){
      throw std::runtime_error("mo requires period >= 1");
    }
    
    auto s = sp.get();
    series_builder builder;
    value_type cycle[N];
    size_t count = 0;
    
    for(auto dt : *current_dates::active()){
      auto val = s->at(dt);
      auto slot = count % N;
      if(count >= N){
	auto old_val = cycle[slot];
	auto ratio = change(val, old_val);
	if(valid(ratio)){
	  builder.insert({ dt, ratio});
	}
      }
      cycle[slot] = val;
      count++;
    }

    return builder.construct();
  }


  series_ptr mo_days (series_ptr sp, unsigned days) {
    
    if (days < 1){
      throw std::runtime_error("mo_days require days >= 1");
    }

    auto fs = fudge(sp);
    auto s = fs.get();
    series_builder builder;

    for (auto dt : *current_dates::active()){
      auto now = s->at(dt);
      auto then = s->at(dt - days);
      auto ratio = change(now, then);
      if(valid(ratio)){
	builder.insert({dt, ratio});
      }
    }
    
    return builder.construct();
  }
 

}
