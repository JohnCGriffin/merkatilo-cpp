

#include "merkatilo.hpp"

#include <cmath>


namespace merkatilo {

  double gpa(series_ptr s, dateset_ptr dates)
  {
    const auto f_ob = first_ob(s, dates);
    const auto l_ob = last_ob(s, dates);

    if(l_ob.dt == f_ob.dt){
      std::out_of_range("no data available to calculate gpa");
    }

    const auto days = l_ob.dt - f_ob.dt;
    const auto years = days / 365.2425;
    const auto gain = l_ob.val / f_ob.val;
    return std::pow(gain, 1.0 / years) - 1.0;
  }

  /// Return the performance structure of trading based upon signals or
  /// simply return the performance of buy/hold if no signals are
  /// supplied.  Don't forgot to warp the signals by one if you want
  /// realistic results of trading the day following end-of-day signals.
  
  performance investment_performance (series_ptr sp,
				      series_ptr signals,
				      series_ptr alternate_investment,
				      dateset_ptr dates)
  {
    const auto equity = equity_line(sp,signals,alternate_investment);

    performance result;
    result.volatility_residual = 1.0 - volatility(equity);
    result.drawdown_residual = ([&](){
	auto dd = series_drawdown(equity);
	return dd.residual();
      })();
    result.annualized_gain = gpa(equity, dates);
    const auto pair = ([&]() -> std::pair<size_t,double> {
	size_t sig_count = 0;
	double total = 0;
	double longs = 0;
	double state = 0;
	auto s = signals.get();
	for(const auto dt : *dates){
	  auto val = s->at(dt);
	  if(valid(val) && val){
	    state = val;
	    sig_count++;
	  }
	  if(state){
	    total++;
	    if (state > 0){
	      longs++;
	    }
	  }
	}
	return std::make_pair(sig_count, (total>0) ? (longs/total) : 0.0);
      })();
    result.trades = pair.first;
    result.long_ratio = pair.second;
    return result;
  }

}
