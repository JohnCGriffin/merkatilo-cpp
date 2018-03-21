
#include "merkatilo.hpp"

namespace merkatilo {

  series::~series(){}

  double series::at(jdate) const { return default_value(); }

  observations_ptr series::observations_by_date(jdate_v_ptr dates) {
    observations result;
    result.reserve(dates->size());
    for (auto dt : *dates){
      result.push_back({dt, this->at(dt) });
    }
    return std::make_shared<const observations>(result);
  }
}
