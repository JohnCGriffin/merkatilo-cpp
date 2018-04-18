
#include "merkatilo.hpp"
#include <algorithm>
#include <functional>

namespace merkatilo {

  static double capture (series_ptr s, series_ptr benchmark, uint period, dateset_ptr dates,
			 std::function<bool(double)> predicate)
  {
    const auto s_mo = mo(s,period,dates);
    const auto b_mo = mo(benchmark, period, dates);
    double total_B = 0;
    double total_S = 0;
    for (const auto dt : *dates){
      const auto B_val = b_mo->at(dt);
      if(valid(B_val) && predicate(B_val)){
	const auto S_val = s_mo->at(dt);
	if(valid(S_val)){
	  total_B += B_val;
	  total_S += S_val;
	}
      }
    }
    if (!total_B){
      throw std::runtime_error("no capture observations");
    }
    return std::max(0.0, (total_S/total_B));
  }

  double down_capture (series_ptr s, series_ptr benchmark, uint period, dateset_ptr dates)
  {
    return capture(s,benchmark,period,dates,[](double a){ return a<0; });
  }
  
  double up_capture (series_ptr s, series_ptr benchmark, uint period, dateset_ptr dates)
  {
    return capture(s,benchmark,period,dates,[](double a){ return a>0; });
  }

}
