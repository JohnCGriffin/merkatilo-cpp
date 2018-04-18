
#include "merkatilo.hpp"

namespace merkatilo {

  /// A common and valid use of warp is to shift signals when generating
  /// an equity line because it may better reflect next day trading on on
  /// daily signals.


  series_ptr warp (series_ptr sp, int N, dateset_ptr dates) {

    const auto obs = sp->observations_by_date(dates);
    series_builder builder;

    int ndx=-1;
    const int len = obs->size();
    
    for (const auto ob : *obs){
      ndx++;
      const auto val = ob.val;
      if(!valid(val)){
	continue;
      }
      const int offset_ndx = ndx + N;
      if(offset_ndx >= 0 && offset_ndx < len){
	auto target_date = obs->at(offset_ndx).dt;
	builder.insert (observation{ target_date, val });
      }
    }

    return builder.construct();
  }

}

