
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

  /// @brief incremental constructor of dateset
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

  /// @brief adds lexical scope to thread local dateset
  class current_dates {
    std::shared_ptr<dateset> previous;
  public:
    current_dates(series_ptr s);
    current_dates(dateset_ptr ds);
    ~current_dates();

    static const /* thread_local */ dateset_ptr active();
  };

  /// @brief first date of date set
  jdate first_date(dateset_ptr);

  /// @brief first date of current date set
  jdate first_date();

  /// @brief last date of date set
  jdate last_date(dateset_ptr);

  /// @brief last date of current date set
  jdate last_date();

  

  struct overloaded_operator_variant {
    series_ptr s;
    overloaded_operator_variant(series_ptr);
    overloaded_operator_variant(double);
  };
  
  // NORMAL arithmetic at every date.  If the second
  // argument is a number, it is converted with
  // constant to make it a series.

  series_ptr operator+(series_ptr a, overloaded_operator_variant b);
  series_ptr operator-(series_ptr a, overloaded_operator_variant b);
  series_ptr operator*(series_ptr a, overloaded_operator_variant b);
  series_ptr operator/(series_ptr a, overloaded_operator_variant b);


  // Inequalities are handy shortcuts to filtering
  // series A if the inequality over A and B holds
  // true.  Thus, (IBM > 100) means to copy all
  // IBM values greater than 100.
  
  series_ptr operator< (series_ptr a, overloaded_operator_variant b);
  series_ptr operator<=(series_ptr a, overloaded_operator_variant b);
  series_ptr operator> (series_ptr a, overloaded_operator_variant b);
  series_ptr operator>=(series_ptr a, overloaded_operator_variant b);

  // The 'or' function composes two series, returning the first
  // if available, else the second.  The 'and' function returns
  // the second if the first is available.  In the same style
  // as the arithmetic overloads, the second argument can
  // be a number.

  series_ptr series_or (series_ptr a, overloaded_operator_variant b);
  series_ptr series_and (series_ptr a, overloaded_operator_variant b);

  
  /// @brief restate series in different units
  /// @param s series
  /// @param init initial value
  /// @param date date at which initial value is applied

  series_ptr calibrate (series_ptr s, double init=100, jdate date=0);

  /// @brief decorate series that finds exact or nearest preceding observtion
  /// @param s series
  /// @param days number of days to look back for data (default 6)
  /// @return decorated series
  
  series_ptr fudge (series_ptr s, unsigned days=6);


  /// @brief series that returns N for any date
  /// @param N the returned value for any date
  /// @return constant series
  
  series_ptr constant (double N);


  /// @brief read text representaion of series
  /// @param is input stream
  /// @return deserialized series
  
  series_ptr read_series(std::istream& is);


  /// @brief filter series based upon predicate
  /// @param s series
  /// @param predicate boolean function for filtering a double.
  /// @return filtered series
  
  series_ptr filter (series_ptr s, std::function<bool(double)> predicate);



  ///////// DATES TRAVERSING SERIES OPS //////////////

  /// @brief fractional smoothing
  /// @param s series to smooth
  /// @param fraction between 0 and 1
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return smoothed series

  series_ptr fractional (series_ptr s, double fraction, dateset_ptr dates=current_dates::active());

  /// @brief exponential smoothing 
  /// @param s series to smooth
  /// @param N used to create fraction 2/(N+1)
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return smoothed series

  series_ptr ema (series_ptr s, unsigned N, dateset_ptr dates=current_dates::active());


  /// @brief simple moving average
  /// @param s series to smooth
  /// @param period number of periods to average
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series containing rolling N-period averages
  
  series_ptr ma (series_ptr s, unsigned period, dateset_ptr dates=current_dates::active());


  /// @brief periodic gains or declines
  /// @param s series
  /// @param period number of periods back from which to measure gain/decline
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series containing (new/old)-1.
  
  series_ptr mo (series_ptr s, unsigned period, dateset_ptr dates=current_dates::active());


  /// @brief gains or declines over specified calendar days
  /// @param s series
  /// @param days calendar days back from which to measure gain/decline
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series containing (new/old)-1
  
  series_ptr mo_days (series_ptr s, unsigned days, dateset_ptr dates=current_dates::active());


  /// @brief shift data backward or forward by N periods.
  /// @param s series
  /// @param N periods forward or backward to shift observations
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series of shifted observations
  
  series_ptr warp (series_ptr s, int N, dateset_ptr dates=current_dates::active());


  /// @brief remove repeated values
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series such that adjacent values are unique.
  
  series_ptr unrepeated (series_ptr s, dateset_ptr dates=current_dates::active());


  /// @brief fill missing values with closest preceding observation
  /// @param s initial series with missing observations to fill
  /// @param repeat_last whether to continue repeated past last observation (default false)
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series with missing observation filled using previous value
  
  series_ptr repeated (series_ptr s, bool repeat_last=false, dateset_ptr dates=current_dates::active());


  /// @brief apply function to N observations
  /// @param s series
  /// @param N size of vector<double> passed to window_function
  /// @param window_function consumes vector<double> of size N, returning a double.
  /// @param missing_data_permitted permits windows containing missing observations (default false)
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series of scalar window_function results

  series_ptr window_series (series_ptr s,
			    unsigned N,
			    std::function<double(const std::vector<double>&)> window_function,
			    bool missing_data_permitted=false,
			    dateset_ptr dates=current_dates::active());


  /// @brief convert values of one or more series into a scalar at each date
  /// @param sps vector of series
  /// @param f function to process all series values on a given date
  /// @param missing_data_permitted whether f should process missing data
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series of functional mapping of one or more input series 

  series_ptr series_map (std::vector<series_ptr> sps,
			 std::function<double(const std::vector<double>&)> f,
			 bool missing_data_permitted=false,
			 dateset_ptr dates=current_dates::active());

  /**
   @brief handy syntax to variable number of series rather than vector of series.
  */
  template <typename... Ss>
  series_ptr series_map(std::function<double(const std::vector<double>&)> f, Ss... ss){
    std::vector<series_ptr> v { std::forward<Ss>(ss)... };
    return series_map(v,f);
  }


  ////////////   SIGNAL GENERATION  ///////////////

  /// @brief convert negative and non-negative values to signals.
  /// @param s input series of positive and negative values.
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return signal series (series of non-repeating -1 and 1)

  series_ptr to_signals (series_ptr s, dateset_ptr dates=current_dates::active());
  

  /// @brief identify when local loss or gain has occurred
  /// @param s series
  /// @param down_factor factor to multiply by peak to indicate sell signal
  /// @param up_factor factor to multiply by trough to indicate buy signal
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return signal series (series of non-repeated -1 and 1).
  
  series_ptr reversals (series_ptr s,
			double down_factor,
			double up_factor,
			dateset_ptr dates=current_dates::active());


  /// @brief suppress unstable signal changes
  /// @param sigs signal series
  /// @param N number of periods required for conviction to endorse signals
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return signal series delayed N periods and endorsed for N periods

  series_ptr conviction (series_ptr sigs, unsigned N, dateset_ptr dates=current_dates::active());


  /// @brief signal when faster-changing series crosses slower-changing series.
  /// @param slower slower moving series (e.g. an sma or ema result)
  /// @param faster faster moving series (e.g. simple price line)
  /// @param upside_factor factor by which upward fast movement crosses slower (default 1.0)
  /// @param down_factor factor by which downward fast movement crosses slower (default 1.0)
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return signal series
  
  series_ptr cross(series_ptr slower, series_ptr faster,
		   double upside_factor=1.0, double downside_factor=1.0,
		   dateset_ptr dates=current_dates::active());


  ////////  EVALUATE SIGNAL PERFORMANCE /////////


  /// @brief construct value of signal-traded series.
  /// @param s series
  /// @param signals signal series
  /// @param alternate_investment where funds are during active sell signal
  /// @param initial_value starting value of funds (default 100)
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return series representing value of trading on supplied signals
  
  series_ptr equity_line(series_ptr s,
			 series_ptr signals,
			 series_ptr alternate_investment = constant(1),
			 double initial_value = 100,
			 dateset_ptr dates=current_dates::active());

  /// @brief collection of investment performance statistics
  struct performance {
    /// volatility residual = 1 - volatility
    double volatility_residual;
    /// drawdown residual = 1 - drawdown
    double drawdown_residual;
    /// annualized gain, e.g. 1.03 for 3% annual gains
    double annualized_gain;
    /// time spent on buy / total time
    double long_ratio;
    /// signal count
    unsigned trades;
  };



  /// @brief return performance attributes of series traded by signals.
  /// @param s series
  /// @param signals buy/sell instructions
  /// @param alternate_investment where funds go during active sell instruction
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return performance struct
  
  performance investment_performance (series_ptr s,
				      series_ptr signals = constant(1),
				      series_ptr alternate_investment = constant(1),
				      dateset_ptr dates = current_dates::active());

  
  /// @brief measure of variation in returns.
  /// @param s series 
  /// @param days number of days to calculate change (default 365)
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return double
  
  double volatility (series_ptr s, unsigned days=365, dateset_ptr dates=current_dates::active());


  /// @brief annualized gain of series
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return annualized gain, e.g. 1.02 for 2% annual compounded gains

  double gpa(series_ptr s, dateset_ptr dates=current_dates::active());


  /// @brief identify largest loss in equity series.
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return largest drop in equity value, e.g. 0.4 means a 40% loss.
  
  obpair drawdown(series_ptr s, dateset_ptr dates=current_dates::active());


  ////////////// MISC ////////////////

  /// @brief observation count
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return integer count of observations
  
  size_t series_count(series_ptr s, dateset_ptr dates=current_dates::active());


  /// @brief return minimum and maximum valued observations.
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return std::pair representing (minimum,maximum)
  
  obpair min_max_obs(series_ptr s, dateset_ptr dates=current_dates::active());


  /// @brief first observation in a series
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return first observation
  /// @throws std::out_of_range upon finding no observation
  
  observation first_ob(series_ptr s, dateset_ptr dates=current_dates::active());

  
  /// @brief last observation in a series
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return last observation
  /// @throws std::out_of_range upon finding no observation
  
  observation last_ob(series_ptr s, dateset_ptr dates=current_dates::active());


  /// @brief show local minima and maxima.
  /// @param s series
  /// @param down_factor amount of descent from local maxima
  /// @param up_factor amount of ascent from local minima
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return signal-like series indicating maxima with -1 and minima with 1
  
  series_ptr nostradamus (series_ptr s,
			  double down_factor,
			  double up_factor,
			  dateset_ptr dates=current_dates::active());

  
  /////////////// CONVERSIONS ////////////


  
  /// @brief convert vector of observations into a series.
  /// @param obs observations
  /// @return series
  
  series_ptr obs_to_series(observations_ptr obs);


  /// @brief convert series into vector of observations
  /// @param s series
  /// @param dates dates to traverse (defaulted to current dates)
  /// @return observations
  
  observations_ptr series_to_obs(series_ptr s, dateset_ptr dates=current_dates::active());

  
  ///////////////  I/O  //////////////////


  /// @brief load time series
  /// @param id TICKER::RECORD style id
  /// @return series
  
  series_ptr lo(std::string id);

  
  /// @brief print spreadsheet style display of one or more series

  void dump(std::initializer_list<series_ptr>);

  /**
     @brief handy variable args call of dump.
  */
  template <typename... Ss>
  void dump(Ss... ss){
    dump ({ std::forward<Ss>(ss)... });
  }

  /// @brief write series to output stream
  
  void write_series(series_ptr, dateset_ptr, std::ostream&);


  struct portion {
    series_ptr sp;
    double amount;
    portion (series_ptr sp, double amount);
  };

  struct allocation {
    jdate date;
    std::vector<portion> portions;
    allocation (jdate date, std::vector<portion>& portions);
  };

  
  series_ptr allocation_equity_line (std::vector<allocation>& allocations, double initial_value);

}

#endif

