
#include "merkatilo.hpp"
#include <cmath>

namespace merkatilo {

  static double avg (const std::vector<double>& vec)
  {
    double total = 0;
    for (auto val : vec){
      total += val;
    }
    return total / vec.size();
  }

  static double stddev (const std::vector<double>& v)
  {
    auto mean = avg(v);
    std::vector<double> squaredDiffs;
    for (auto val : v){
      auto diff = mean - val;
      squaredDiffs.push_back(diff * diff);
    }
    return std::sqrt(avg(squaredDiffs));
  }

  double volatility (series_ptr sp, unsigned days) {

    auto changes = mo_days(sp, days);
    auto obs = changes->observations_by_date(current_dates::active());

    std::vector<double> v;
    for (auto ob : *obs){
      if(valid(ob.val)){
	v.push_back(ob.val);
      }
    }

    return stddev(v);
  }


}
