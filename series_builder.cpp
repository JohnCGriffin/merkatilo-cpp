
#include "merkatilo.hpp"
#include <cmath>
#include <memory>
#include <algorithm>

namespace merkatilo {

  static size_t last_build_size = 0;
  
  series_builder& series_builder::insert(observation ob){
    const auto sz = obs.size();
    if(!sz && last_build_size){
      obs.reserve(last_build_size);
    }
    if(sz && ob.dt <= obs[sz-1].dt){
      ordered = false;
    }
    obs.push_back(ob);
    return *this;
  }

  typedef std::pair<jdate_v_ptr,observations_ptr> obs_cache;

  struct series_builder_series : public series {
    std::unique_ptr<obs_cache> cache;
    const double_v v;
    jdate fd;
    jdate ld;
    series_builder_series(const double_v& v, jdate fd, jdate ld)
      : v(v), fd(fd), ld(ld) {}
    double at(jdate jd) const override {
      if(jd < fd || ld < jd){
	return default_value();
      }
      return v.at(jd - fd);
    }
    observations_ptr observations_by_date (jdate_v_ptr dates) override {
      observations_ptr result;
      std::unique_ptr<obs_cache> retriever;
      retriever.swap(cache);
      auto const p = retriever.get();
      if(p != nullptr && p->first.get() == dates.get()){
	result = p->second;
      } else {
	result = series::observations_by_date(dates);
	retriever = std::make_unique<obs_cache>(dates,result);
      }
      retriever.swap(cache);
      return result;
    }
  };

  

  series_ptr series_builder::construct()
  {
    //bool is_signal_series = true;
    if(!ordered){
      std::sort(obs.begin(), obs.end());
    }
    /*
    for(unsigned int i=0; i<obs.size() && is_signal_series; i++){
      if(std::abs(obs.at(i).val) != 1.0){
	is_signal_series = false;
      }
      if(i > 0 && obs.at(i).val != -(obs.at(i-1).val)){
	is_signal_series = false;
      }
    }
    */
    if(obs.empty()){
      return std::make_shared<series>();
    }
    const auto fd = obs.begin()->dt;
    const auto ld = obs.rbegin()->dt;
    double_v v(1 + (ld-fd), default_value());
    /*
    v.reserve(1 + (ld - fd));
    for(auto dt = fd; dt <= ld; dt = dt+1){
      v.push_back(default_value());
    }
    */
    for(const auto& ob : obs){
      const int slot = ob.dt - fd;
      v.at(slot) = ob.val;
    }
    last_build_size = obs.size();
    return std::make_shared<series_builder_series>(v,fd,ld);
  }
  
}

