
#include "merkatilo.hpp"


namespace merkatilo {

  static value_type change (value_type val, value_type old_val){
    if(!valid(old_val) || !valid(val) || old_val == 0){
      return default_value();
    }
    return (val / old_val) - 1.0;
  }

/**
 *@brief periodic ratio of (new/old)-1.  Thus, a 10% gain is 0.1 and a 2% loss
 * is -0.02.  
 */

  series_ptr mo (series_ptr sp, unsigned N, dateset_ptr dates) {

    if(N < 1){
      throw std::runtime_error("mo requires period >= 1");
    }
    
    auto s = sp.get();
    series_builder builder;
    value_type cycle[N];
    size_t count = 0;
    
    for(auto dt : *dates){
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


/**
 *@brief Looking back specified number of days, calculate ratio of (new/old)-1.  Thus, a 10% gain is 0.1 and a 2% loss
 * is -0.02.  Thus, one year change is mo_days(some_price_series, 365).  If the earlier date falls on a non-market day,
 * the previous market day is used.
 */

  series_ptr mo_days (series_ptr sp, unsigned days, dateset_ptr dates) {
    
    if (days < 1){
      throw std::runtime_error("mo_days require days >= 1");
    }

    auto fs = fudge(sp);
    auto s = fs.get();
    series_builder builder;

    for (auto dt : *dates){
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
