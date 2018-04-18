
#include "merkatilo.hpp"

namespace merkatilo {

  /// Optionally, one can terminate at the last valid value or continue it
  /// to the end of the current dateset.  If you simply need data to fit
  /// together as in month-end and market data, using fudge may be a
  /// better option.

  series_ptr repeated (series_ptr sp, bool repeat_last, dateset_ptr dates){

    const auto s = sp.get();
    observations obs;
    observation last_ob { 0, default_value() };

    for (const auto dt : *dates){

     const auto val = s->at(dt);

      if(valid(val)){
	last_ob = { dt, val };
	obs.push_back(last_ob);
	
      } else if (last_ob.dt) {
	obs.push_back({ dt, last_ob.val });
	
      }
      
    }

    series_builder builder;

    for (const auto& ob : obs){
      if(repeat_last || ob.dt <= last_ob.dt){
	builder.insert(ob);
      }
    }

    return builder.construct();
  }

}
