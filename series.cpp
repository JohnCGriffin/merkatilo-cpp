
#include "merkatilo.hpp"

namespace merkatilo {

  series::~series(){}

  opt_double series::at(jdate) const { return {}; }

  opt_double_v_ptr series::values_of_dates(jdate_v_ptr dates) const {
    opt_double_v result;
    result.reserve(dates->size());
    for (auto dt : *dates){
      result.push_back(this->at(dt));
    }
    return std::make_shared<opt_double_v>(result);
  }
}
