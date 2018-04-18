
#include "merkatilo.hpp"

namespace merkatilo {


  class filter_series : public series {
    const series_ptr sp;
    const std::function<bool(double)> predicate;
  public:
    filter_series (series_ptr sp, std::function<bool(double)> predicate)
      : sp(sp), predicate(predicate){}
    double at(jdate dt) const override {
      const auto val = sp->at(dt);
      return (valid(val) && predicate(val)) ? val : default_value();
    }
  };
  
  /**
     @brief Filter series by predicate.
     *
     * Each value produced by the input series is passed to 
     * the output series unchanged if that value                                
     * passes the predicate. 
     */
  series_ptr filter (series_ptr sp, std::function<bool(double)> predicate)
  {
    return std::make_shared<filter_series>(sp,predicate);
  }

}
