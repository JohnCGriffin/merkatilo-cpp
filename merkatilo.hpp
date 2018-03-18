
#ifndef _MERKATILO_HPP_293487293847_
#define _MERKATILO_HPP_293487293847_

/** @file */

#include <set>
#include <string>
#include <memory>
#include <limits>
#include <vector>
#include <functional>
#include <sstream>

#include "jdate.hpp"
#include "series.hpp"

namespace merkatilo {

  typedef const std::vector<jdate> dateset;
  typedef std::shared_ptr<dateset> dateset_ptr;

  class dateset_builder {
    std::set<jdate> _so_far;
    jdate _low;
    jdate _high;
  public:
    dateset_builder(dateset_ptr);
    dateset_builder(series_ptr);
    dateset_builder(jdate low, jdate high);
    dateset_builder& intersect(dateset_ptr);
    dateset_builder& intersect(series_ptr);
    dateset_builder& add(dateset_ptr);
    dateset_builder& add(series_ptr);
    dateset_builder& low_date(jdate);
    dateset_builder& high_date(jdate);
    dateset construct();
  };

  class current_dates {
    std::shared_ptr<dateset> previous;
  public:
    current_dates(series_ptr s);
    current_dates(dateset_ptr ds);
    ~current_dates();

    static const /* thread_local */ dateset_ptr active();
  };


  
  // NORMAL arithmetic at every date.  If the second
  // argument is a number, it is converted with
  // constant to make it a series.

  series_ptr operator+(series_ptr a, series_ptr b);
  series_ptr operator+(series_ptr a, double b);
  
  series_ptr operator-(series_ptr a, series_ptr b);
  series_ptr operator-(series_ptr a, double b);

  series_ptr operator*(series_ptr a, series_ptr b);
  series_ptr operator*(series_ptr a, double b);
  
  series_ptr operator/(series_ptr a, series_ptr b);
  series_ptr operator/(series_ptr a, double b);

  // Inequalities are handy shortcuts to filtering
  // series A if the inequality over A and B holds
  // true.  Thus, (IBM > 100) means to copy all
  // IBM values greater than 100.
  
  series_ptr operator<(series_ptr a, series_ptr b);
  series_ptr operator<(series_ptr a, double b);
  
  series_ptr operator<=(series_ptr a, series_ptr b);
  series_ptr operator<=(series_ptr a, double b);
  
  series_ptr operator>(series_ptr a, series_ptr b);
  series_ptr operator>(series_ptr a, double b);
  
  series_ptr operator>=(series_ptr a, series_ptr b);
  series_ptr operator>=(series_ptr a, double b);

  // The 'or' function composes two series, returning the first
  // if available, else the second.  The 'and' function returns
  // the second if the first is available.  In the same style
  // as the arithmetic overloads, the second argument can
  // be a number.

  series_ptr series_or (series_ptr a, series_ptr b);
  series_ptr series_or (series_ptr a, double b);

  series_ptr series_and (series_ptr a, series_ptr b);
  series_ptr series_and (series_ptr a, double b);

  series_ptr fudge (series_ptr, unsigned days=6);
  series_ptr constant (double N);
  series_ptr read_series(std::istream& is);
  series_ptr filter (series_ptr, std::function<bool(double)> predicate);



  ///////// DATES TRAVERSING SERIES OPS //////////////

/**
@brief exponential moving average
*
* Using N > 1, a fraction is calculated FRAC = (2 / (N+1)).  That weight
* is given to a current value with the remainder (1-FRAC) given to the 
* preceding date's calculation.  Observations with no immediately preceding
* observation simply pass through the current value.
*/

  series_ptr ema (series_ptr, unsigned N, dateset_ptr dates=current_dates::active());
  series_ptr sma (series_ptr, unsigned period, dateset_ptr dates=current_dates::active());
  series_ptr mo (series_ptr, unsigned period, dateset_ptr dates=current_dates::active());
  series_ptr mo_days (series_ptr, unsigned days, dateset_ptr dates=current_dates::active());
  series_ptr warp (series_ptr, int N, dateset_ptr dates=current_dates::active());
  series_ptr unrepeated (series_ptr, dateset_ptr dates=current_dates::active());
  series_ptr repeated (series_ptr, bool repeat_last=false, dateset_ptr dates=current_dates::active());

  series_ptr window_series (series_ptr sp,
			    unsigned N,
			    std::function<double(const std::vector<double>&)> window_function,
			    bool missing_data_permitted=false,
			    dateset_ptr dates=current_dates::active());

  series_ptr series_map (std::vector<series_ptr> sps,
			 std::function<double(const std::vector<double>&)> f,
			 bool missing_data_permitted=false,
			 dateset_ptr dates=current_dates::active());

  /**
   @brief handy syntax to variable number of series.
  */
  template <typename... Ss>
  series_ptr series_map(std::function<double(const std::vector<double>&)> f, Ss... ss){
    std::vector<series_ptr> v { std::forward<Ss>(ss)... };
    return series_map(v,f);
  }



  ////////////   SIGNAL GENERATION  ///////////////
  
  series_ptr to_signals (series_ptr, dateset_ptr dates=current_dates::active());
  
  series_ptr reversals (series_ptr,
			double down_factor,
			double up_factor,
			dateset_ptr dates=current_dates::active());


  series_ptr conviction (series_ptr, unsigned N, dateset_ptr dates=current_dates::active());
  
  series_ptr cross(series_ptr slower, series_ptr faster,
		   double upside_factor=1.0, double downside_factor=1.0,
		   dateset_ptr dates=current_dates::active());


  ////////  EVALUATE SIGNAL PERFORMANCE /////////

  series_ptr equity_line(series_ptr sp,
			 series_ptr signals,
			 series_ptr alternate_investment = constant(1),
			 double initial_value = 100,
			 dateset_ptr dates=current_dates::active());

  struct performance {
    double volatility_residual;
    double drawdown_residual;
    double annualized_gain;
    double long_ratio;
    unsigned trades;
  };

  performance investment_performance (series_ptr s,
				      series_ptr signals = constant(1),
				      series_ptr alternate_investment = constant(1),
				      dateset_ptr dates = current_dates::active());
  
  double volatility (series_ptr, unsigned days=365, dateset_ptr dates=current_dates::active());
  double gpa(series_ptr, dateset_ptr dates=current_dates::active());
  obpair drawdown(series_ptr, dateset_ptr dates=current_dates::active());


  ////////////// MISC ////////////////

  size_t series_count(series_ptr, dateset_ptr dates=current_dates::active());
  obpair min_max_obs(series_ptr, dateset_ptr dates=current_dates::active());
  observation first_ob(series_ptr, dateset_ptr dates=current_dates::active());
  observation last_ob(series_ptr, dateset_ptr dates=current_dates::active());

  /* nostradamus is based upon reversals except that that date of each
     observation in the output is the local extreme that preceded the
     reversal.  DO NOT mistake this for a trading signal. */
  
  series_ptr nostradamus (series_ptr,
			  double down_factor,
			  double up_factor,
			  dateset_ptr dates=current_dates::active());

  
  /////////////// CONVERSIONS ////////////
  
  series_ptr obs_to_series(observations_ptr);
  observations_ptr series_to_obs(series_ptr, dateset_ptr dates=current_dates::active());

  
  ///////////////  I/O  //////////////////

  series_ptr lo(std::string id);
  void dump(std::initializer_list<series_ptr>);

  /**
     @brief handy variable args call of dump.
  */
  template <typename... Ss>
  void dump(Ss... ss){
    dump ({ std::forward<Ss>(ss)... });
  }

  void write_series(series_ptr, dateset_ptr, std::ostream&);
}

#endif

