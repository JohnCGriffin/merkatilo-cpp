
#include "merkatilo.hpp"

namespace merkatilo {

  static dateset series_to_dateset(const series& s)
  {
    std::set<jdate> result;
    for(auto dt = jdate::MIN_DATE; dt < jdate::MAX_DATE; dt = dt+1){
      if (s(dt)){
	result.insert(dt);
      }
    }
    return result;
  }

  static dateset range_to_dateset(jdate low, jdate high)
  {
    std::set<jdate> result;
    while(low <= high){
      result.insert(low);
      low = low+1;
    }
    return result;
  }

  dateset_builder::dateset_builder(dateset& ds) : _so_far(ds) {}

  dateset_builder::dateset_builder(series& s) : _so_far(series_to_dateset(s)) {}

  dateset_builder::dateset_builder(jdate low, jdate high)
    : _so_far(range_to_dateset(low,high)), _low(low), _high(high) {}


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

  dateset_builder& dateset_builder::intersect(dateset& ds){
    _so_far = _intersect(_so_far, [&](jdate dt){ return ds.find(dt) != ds.end(); });
    return *this;
  }

  dateset_builder& dateset_builder::intersect(series& s){
    _so_far = _intersect(_so_far, [&](jdate jd){ return s(jd) ? true : false; });
    return *this;
  }

  dateset_builder& dateset_builder::add(dateset& ds){
    for(auto jd : ds){
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

  dateset_builder& dateset_builder::add(series& s){
    auto low = _low ? *_low : jdate::MIN_DATE;
    auto high = _high ? *_high : jdate::MAX_DATE;
    while (low <= high){
      if(s(low)){
	_so_far.insert(low);
      }
      low = low + 1;
    }
    return *this;
  }

  dateset dateset_builder::construct() {
    return _so_far;
  }

}
