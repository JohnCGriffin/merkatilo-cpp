
#include "merkatilo.hpp"
#include <sstream>

namespace merkatilo {

  struct calibrate_series : public series {
    series_ptr s;
    double ratio;
    calibrate_series(series_ptr s, double ratio) : s(s), ratio(ratio){}
    double at(jdate jd) const override {
      const auto val = s->at(jd);
      return valid(val) ? (val * ratio) : val;
    }
  };

  /// To calibrate a series is to make it value-compatible with another.
  /// For example, a spider graph picks a point at which multiple time
  /// series are the same value.  The default date is the beginning of the
  /// dateset and the default value of the series there is 100, making any
  /// value in the series equivalent to the percentage of the beginning.
    
  series_ptr calibrate (series_ptr s, double init, jdate d)
  {
    const auto date = d ? d : (current_dates::active()->at(0));
    const auto val = s->at(date);
    if(!valid(val)){
      std::ostringstream oss;
      oss << "missing observation at " << jdate_to_string(date);
      throw std::out_of_range(oss.str());
    }
    const auto ratio = init / val;
    return std::make_shared<calibrate_series>(s,ratio);
  }

}
