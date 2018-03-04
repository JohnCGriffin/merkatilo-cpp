
#include "merkatilo.hpp"

/**@file
 *@brief periodic ratio of new/old
 */

namespace merkatilo {

  static value_type change (value_type val, value_type old_val){
    if(!valid(old_val) || !valid(val) || old_val == 0){
      return default_value();
    }
    return (val / old_val) - 1.0;
  }

  series_ptr mo(series_ptr sp, unsigned N){

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

}
