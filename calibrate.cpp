
#include "merkatilo.hpp"
#include <sstream>

namespace merkatilo {

  struct calibrate_series : public series {
    series_ptr s;
    double ratio;
    calibrate_series(series_ptr s, double ratio) : s(s), ratio(ratio){}
    double at(jdate jd) const override {
      auto val = s->at(jd);
      return valid(val) ? (val * ratio) : val;
    }
  };

  series_ptr calibrate (series_ptr s, double init, jdate d)
  {
    auto date = d ? d : (current_dates::active()->at(0));
    auto val = s->at(date);
    if(!valid(val)){
      std::ostringstream oss;
      oss << "missing observation at " << jdate_to_string(date);
      throw std::runtime_error(oss.str());
    }
    auto ratio = init / val;
    return std::make_shared<calibrate_series>(s,ratio);
  }

}
