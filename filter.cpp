
#include "merkatilo.hpp"

namespace merkatilo {

  class filter_series : public series {
    series_ptr sp;
    std::function<bool(double)> predicate;
  public:
    filter_series (series_ptr sp, std::function<bool(double)> predicate)
      : sp(sp), predicate(predicate){}
    double at(jdate dt) const override {
      auto val = sp->at(dt);
      return (valid(val) && predicate(val)) ? val : default_value();
    }
  };
  
  series_ptr filter (series_ptr sp, std::function<bool(double)> predicate)
  {
    return std::make_shared<filter_series>(sp,predicate);
  }

}
