
#include "merkatilo.hpp"
#include <cmath>

namespace merkatilo {

  static double avg (const std::vector<double>& vec)
  {
    double total = 0;
    for (const auto& val : vec){
      total += val;
    }
    return total / vec.size();
  }

  static double stddev (const std::vector<double>& v)
  {
    const auto mean = avg(v);
    double totalDiff = 0.0;
    for (const auto val : v){
      auto diff = mean - val;
      totalDiff += (diff * diff);
    }
    auto avgDiff = totalDiff / v.size();
    return std::sqrt(avgDiff);
  }

  /// Defaulting to 365 days, return the standard deviation of
  /// the ratio (new/old) where old is 'days' earlier.  
  
  double volatility (series_ptr sp, unsigned days, dateset_ptr dates) {

    const auto changes = mo_days(sp, days);
    const auto obs = changes->observations_by_date(dates);

    std::vector<double> v;
    v.reserve(obs->size());
    
    for (const auto& ob : *obs){
      if(valid(ob.val)){
	v.push_back(ob.val);
      }
    }

    return stddev(v);
  }


}
