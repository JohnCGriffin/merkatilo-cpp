

#include "merkatilo.hpp"

#include <cmath>


namespace merkatilo {

  /**
     @brief annualized gain.
  */

  double gpa(series_ptr s)
  {
    auto f_ob = first_ob(s);
    auto l_ob = last_ob(s);

    if(l_ob.dt == f_ob.dt){
      std::runtime_error("no data available to calculate gpa");
    }

    auto days = l_ob.dt - f_ob.dt;
    auto years = days / 365.2425;
    auto gain = l_ob.val / f_ob.val;
    return std::pow(gain, 1.0 / years) - 1.0;
  }

  /**
     @brief return performance attributes of series traded by signals.
     *
     * Return the performance structure of trading based upon signals or                                                             
     * simply return the performance of buy/hold if no signals are supplied.  Don't forgot                                           
     * to warp the signals by one if you want realistic results of trading the day following
     * end-of-day signals.                                    
     */
  
  performance investment_performance (series_ptr sp,
				      series_ptr signals,
				      series_ptr alternate_investment)
  {
    auto equity = equity_line(sp,signals,alternate_investment);

    performance result;
    result.volatility_residual = 1.0 - volatility(equity);
    result.drawdown_residual = ([&](){
	auto dd = drawdown(equity);
	return dd.second.val / dd.first.val;
      })();
    result.annualized_gain = gpa(equity);
    result.trades = series_count(signals);
    result.long_ratio = ([&](){
	auto filled = repeated(signals, true);
	auto f = filled.get();
	double longs = 0;
	double total = 0;
	for (auto dt : *current_dates::active()){
	  auto val = f->at(dt);
	  if(!valid(val)){
	    continue;
	  }
	  if(val > 0){
	    longs++;
	  } 
	  total++;
	}
	return longs / total;
      })();
    return result;
  }

}
