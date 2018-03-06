
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr warp (series_ptr sp, int N) {

    auto obs = sp->observations_by_date(current_dates::active());
    series_builder builder;

    int ndx=-1;
    int len = obs->size();
    
    for (auto ob : *obs){
      ndx++;
      auto val = ob.val;
      if(!valid(val)){
	continue;
      }
      int offset_ndx = ndx + N;
      if(offset_ndx >= 0 && offset_ndx < len){
	auto target_date = obs->at(offset_ndx).dt;
	builder.insert (observation{ target_date, val });
      }
    }

    return builder.construct();
  }

}

