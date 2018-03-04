
#include "merkatilo.hpp"

namespace merkatilo {

size_t series_count(series_ptr sp)
{
  auto p = sp.get();
  size_t result = 0;
  for(auto dt : *current_dates::active()){
    if(valid(p->at(dt))){
      result++;
    }
  }
  return result;
}

}
