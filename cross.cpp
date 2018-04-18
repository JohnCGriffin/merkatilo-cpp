
#include "merkatilo.hpp"

#include <sstream>



namespace merkatilo {

  /// When the faster series surpasses the slower series, a buy signal (1)
  /// fires.  Likewise, a sell (-1) fires when it goes below.  These
  /// boundaries are altered with upside-factor and downside-factor.
  /// For instance, given a fast series FS and a slow series SS, the
  /// following call will generate buy signals when the fast series exceeds
  /// the slow by 3% and sell signals when it drops 2% below:
  ///
  /// <c>cross(SS,FS,1.03,0.98)</c>
  

  series_ptr cross(series_ptr slower_sp,
		   series_ptr faster_sp,
		   double upside_factor,
		   double downside_factor,
		   dateset_ptr dates)
  {
    auto reasonable_check = [](std::string name, double value, double low, double high){
      if(value < low || value > high){
	std::ostringstream oss;
	oss << name << " of " << value << " outside reasonable [" << low << "," << high << "]";
	throw std::invalid_argument(oss.str());
      }
    };

    reasonable_check("upside_factor", upside_factor, .8, 1.5);
    reasonable_check("downside_factor", downside_factor, .5, 1.2);

    const auto slower = slower_sp.get();
    const auto faster = faster_sp.get();

    double prev_sig = 0.0;
    series_builder builder;

    for (const auto dt : *dates){
      
      const auto s_val = slower->at(dt);
      const auto f_val = faster->at(dt);
      
      if(!valid(s_val) || !valid(f_val)){
	continue;
      }

      const auto sig = (f_val > (upside_factor * s_val)) ? 1.0
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
