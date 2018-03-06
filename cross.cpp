
#include "merkatilo.hpp"

#include <sstream>


namespace merkatilo {

  series_ptr cross(series_ptr slower_sp,
		   series_ptr faster_sp,
		   double upside_factor,
		   double downside_factor)
  {
    auto reasonable_check = [](std::string name, double value, double low, double high){
      if(value < low || value > high){
	std::ostringstream oss;
	oss << name << " of " << value << " outside reasonable [" << low << "," << high << "]";
	throw std::runtime_error(oss.str());
      }
    };

    reasonable_check("upside_factor", upside_factor, .8, 1.5);
    reasonable_check("downside_factor", downside_factor, .5, 1.2);

    auto dts = current_dates::active();
    auto slower = slower_sp.get();
    auto faster = faster_sp.get();

    double prev_sig = 0.0;
    series_builder builder;

    for (auto dt : *dts){
      
      auto s_val = slower->at(dt);
      auto f_val = faster->at(dt);
      
      if(!valid(s_val) || !valid(f_val)){
	continue;
      }

      auto sig = (f_val > (upside_factor * s_val)) ? 1.0
	: (f_val < (downside_factor * s_val)) ? -1.0
	: prev_sig;

      if (sig != prev_sig){
	builder.insert({dt, sig});
      }

      prev_sig = sig;
    }

    return builder.construct();
  }

  

}
