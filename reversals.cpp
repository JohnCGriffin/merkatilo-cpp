
#include "merkatilo.hpp"

namespace merkatilo {

  
  static series_ptr reversals_worker (dateset_ptr dates,
			       series_ptr sp,
			       double down_factor,
			       double up_factor,
			       bool nostradamus)
  {
    enum STATE { NOSIG=0, BUY=1, SELL=-1 };
    auto min_ob = first_ob(sp);
    auto max_ob = min_ob;
    auto obs = sp->observations_by_date(dates);
    STATE state = NOSIG;
    series_builder builder;

    for (auto ob : *obs){

      if(!valid(ob.val)){
	continue;
      }

      if (state != BUY && ob.val > (up_factor * min_ob.val)){
	auto dt = nostradamus ? min_ob.dt : ob.dt;
	min_ob = max_ob = ob;
	state = BUY;
	builder.insert({dt, double(state)});
	
      } else if(state != SELL && ob.val < (down_factor * max_ob.val)){
	auto dt = nostradamus ? max_ob.dt : ob.dt;
	min_ob = max_ob = ob;
	state = SELL;
	builder.insert({dt, double(state)});
	
      } else if (min_ob.val > ob.val){
	min_ob = ob;

      } else if (max_ob.val < ob.val){
	max_ob = ob;

      }

    }

    return builder.construct();
  }

  series_ptr reversals(series_ptr sp,
		       double down_factor,
		       double up_factor)
  {
    return reversals_worker(current_dates::active(), sp, down_factor,up_factor, false);
  }

  series_ptr nostradamus(series_ptr sp,
		       double down_factor,
		       double up_factor)
  {
    return reversals_worker(current_dates::active(), sp, down_factor,up_factor, true);
  }

}
