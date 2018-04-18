
#include "merkatilo.hpp"

namespace merkatilo {

  /**
     @brief return number of observations in series.
  */

  size_t series_count(series_ptr sp, dateset_ptr dates)
  {
    const auto p = sp.get();
    size_t result = 0;
    for(const auto dt : *dates){
      if(valid(p->at(dt))){
	result++;
      }
    }
    return result;
  }

}
