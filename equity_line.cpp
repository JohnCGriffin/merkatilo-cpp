
#include "merkatilo.hpp"

namespace merkatilo {

  /**
     @brief construct value of signal-traded series.
     *
     * If an alternate investment is not specified, it is set to constant(1) and funds are                                 
     * set to initial_value dollars.  When a buy signal arrives, all funds purchase the series                              
     * at its price (the value of the series at that signal date), resulting in shares.  Upon                                    
     * a sell signal, the shares are liquidated and used to purchase the alternate investment at                                 
     * its price.  With initial_value at its default of 100, the value of the output series                                 
     * should the accumulated percentage value related to the original investment.  Until the                                    
     * first signal is encountered, funds are idle, invested in neither the primary nor alternate                                
     * investment.  A graphed equity line will therefore always start with a flat zone before                                    
     * the first signal, as funds are in neither in the primary nor alternate investments.                                       
                                                                                                                              
     * The output of equity_line is what you will likely use with the performance measures                                  
     * of gain, drawdown and volatility.                                                                                         
     */
  
  series_ptr equity_line(series_ptr investment_sp,
			 series_ptr signals_sp,
			 series_ptr alternate_investment_sp,
			 double initial_value,
			 dateset_ptr dates)
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

    for (auto dt : *dates){

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
