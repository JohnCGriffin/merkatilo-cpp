
#include "merkatilo.hpp"

namespace merkatilo {

  /// Optionally, one can terminate at the last valid value or continue it
  /// to the end of the current dateset.  If you simply need data to fit
  /// together as in month-end and market data, using fudge may be a
  /// better option.

  series_ptr repeated (series_ptr sp, bool repeat_last, dateset_ptr dates){

    auto boundary_date = repeat_last ? (last_date(dates)+1) : (last_ob(sp,dates).dt+1);

    const auto s = sp.get();
    observation last_ob { 0, default_value() };

    series_builder builder;

    for (const auto dt : *dates){

      if (dt >= boundary_date){
	break;
      }

      const auto val = s->at(dt);

      if(valid(val)){
	last_ob = { dt, val };
	builder.insert(last_ob);
	
      } else if (last_ob.dt) {
	builder.insert({ dt, last_ob.val });
	
      }
      
    }

    return builder.construct();
  }

}
