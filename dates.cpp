
#include "merkatilo.hpp"

#include <functional>

namespace merkatilo {

  jdate first_date(dateset_ptr ds){
    return ds->at(0);
  }

  /// @brief first date of current date set
  jdate first_date(){
    return first_date(current_dates::active());
  }

  /// @brief last date of date set
  jdate last_date(dateset_ptr ds){
    return ds->at(ds->size()-1);
  }

  /// @brief last date of current date set
  jdate last_date() {
    return last_date(current_dates::active());
  }

  


  static std::set<jdate> series_to_set(const series_ptr s)
  {
    std::set<jdate> result;
    for(auto dt = MIN_DATE; dt < MAX_DATE; dt = dt+1){
      if (valid(s->at(dt))){
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

  dateset_builder::dateset_builder(dateset_ptr ds)
    : _so_far(ds->begin(),ds->end()),_low(0),_high(0) {}

  dateset_builder::dateset_builder(series_ptr s)
    : _so_far(series_to_set(s)),_low(0),_high(0) {}

  dateset_builder::dateset_builder(jdate low, jdate high)
    : _so_far(range_to_set(low,high)), _low(low), _high(high) {}


  static std::set<jdate> _intersect(std::set<jdate> initial, std::function<bool(jdate)> f)
  {
    std::set<jdate> result;
    for(const auto jd : initial){
      if(valid(f(jd))){
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
    _so_far = _intersect(_so_far, [&](jdate jd){ return valid(s->at(jd)); });
    return *this;
  }

  dateset_builder& dateset_builder::add(dateset_ptr ds){
    for(const auto jd : *ds){
      if(_low && jd < _low){
	continue;
      }
      if(_high && _high < jd){
	continue;
      }
      _so_far.insert(jd); 
    }
    return *this;
  }

  dateset_builder& dateset_builder::add(series_ptr s){
    auto low = _low ? _low : MIN_DATE;
    auto high = _high ? _high : MAX_DATE;
    while (low <= high){
      if(valid(s->at(low))){
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
