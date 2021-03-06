
#include "merkatilo.hpp"

namespace merkatilo {

  class fudge_series : public series {
    const series_ptr base;
    const unsigned days;
  public:
    fudge_series(series_ptr base, unsigned days) : base(base), days(days){}
    double at(jdate dt) const override {
      const auto s = base.get();
      for(unsigned i=0; i<=days; i++){
	const auto back_date = dt - i;
	const auto val = s->at(back_date);
	if(valid(val)){
	  return val;
	}
      }
      return default_value();
    }
  };

  series_ptr fudge (series_ptr sp, unsigned days) {
    return std::make_shared<fudge_series>(sp,days);
  }

}

