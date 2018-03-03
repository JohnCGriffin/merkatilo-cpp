
#include "merkatilo.hpp"

namespace merkatilo {

  static std::set<jdate> series_to_set(const series_ptr s)
  {
    std::set<jdate> result;
    for(auto dt = MIN_DATE; dt < MAX_DATE; dt = dt+1){
      if (s->at(dt)){
	result.insert(dt);
      }
    }
    return result;
  }

  static std::set<jdate> range_to_set(jdate low, jdate high)
  {
    std::set<jdate> result;
    while(low <= high){
      result.insert(low);
      low = low+1;
    }
    return result;
  }

  dateset_builder::dateset_builder(dateset_ptr ds) : _so_far(ds->begin(),ds->end()) {}

  dateset_builder::dateset_builder(series_ptr s) : _so_far(series_to_set(s)) {}

  dateset_builder::dateset_builder(jdate low, jdate high)
    : _so_far(range_to_set(low,high)), _low(low), _high(high) {}


  static std::set<jdate> _intersect(std::set<jdate> initial, std::function<bool(jdate)> f)
  {
    std::set<jdate> result;
    for(auto jd : initial){
      if(f(jd)){
	result.insert(jd);
      }
    }
    return result;
  }

  dateset_builder& dateset_builder::intersect(dateset_ptr ds){
    auto s = std::set<jdate>(ds->begin(),ds->end());
    _so_far = _intersect(_so_far, [&](jdate dt){ return s.find(dt) != s.end(); });
    return *this;
  }

  dateset_builder& dateset_builder::intersect(series_ptr s){
    _so_far = _intersect(_so_far, [&](jdate jd){ return s->at(jd) ? true : false; });
    return *this;
  }

  dateset_builder& dateset_builder::add(dateset_ptr ds){
    for(auto jd : *ds){
      if(_low && jd < *_low){
	continue;
      }
      if(_high && *_high < jd){
	continue;
      }
      _so_far.insert(jd); 
    }
    return *this;
  }

  dateset_builder& dateset_builder::add(series_ptr s){
    auto low = _low ? *_low : MIN_DATE;
    auto high = _high ? *_high : MAX_DATE;
    while (low <= high){
      if(s->at(low)){
	_so_far.insert(low);
      }
      low = low + 1;
    }
    return *this;
  }

  dateset dateset_builder::construct() {
    return std::vector<jdate>(_so_far.begin(), _so_far.end());
  }

}
