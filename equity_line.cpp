
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr equity_line(series_ptr investment_sp,
			 series_ptr signals_sp,
			 series_ptr alternate_investment_sp,
			 double initial_value)
  {
    auto investment = investment_sp.get();
    auto signals = signals_sp.get();
    auto alternate_investment = alternate_investment_sp.get();
    
    const jdate first_sig_date = first_ob(signals_sp).dt;
    double product = 1.0;
    double prev_inv = 0;
    double prev_alt = 0;
    bool buy = true;

    series_builder builder;

    for (auto dt : *current_dates::active()){

      const auto inv_val = investment->at(dt);
      const auto sig_val = signals->at(dt);
      const auto alt_val = alternate_investment->at(dt);

      if(valid(sig_val)){
	if(!valid(inv_val)){
	  throw std::runtime_error("investment has no observation on signal date " + jdate_to_string(dt));
	}
	if(!valid(alt_val)){
	  throw std::runtime_error("alternate investment has no observation on signal date " + jdate_to_string(dt));
	}
      }

      if(!valid(inv_val) || !valid(alt_val)){
	continue;
      }

      double change
	= (dt <= first_sig_date) ? 1.0
	: (buy && prev_inv)     ? (inv_val / prev_inv)
	: (!buy && prev_alt)    ? (alt_val / prev_alt)
	: 1.0;

      prev_alt = alt_val;
      prev_inv = inv_val;
      product *= change;
      if(valid(sig_val)){
	buy = !(sig_val < 0);
      }

      builder.insert({dt, product * initial_value});
    }
    
    return builder.construct();
  }

}
