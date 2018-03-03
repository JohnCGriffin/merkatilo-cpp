
#include "merkatilo.hpp"
#include <cmath>
#include <memory>
#include <algorithm>

namespace merkatilo {

  static size_t last_build_size = 0;
  
  void series_builder::insert(observation ob){
    auto sz = obs.size();
    if(!sz && last_build_size){
      obs.reserve(last_build_size);
    }
    if(sz && ob.dt <= obs[sz-1].dt){
      ordered = false;
    }
    obs.push_back(ob);
  }

  typedef std::pair<jdate_v_ptr,observations_ptr> obs_cache;
  
  struct series_builder_series : public series {
    std::unique_ptr<obs_cache> cache;
    const value_type_v v;
    jdate fd;
    jdate ld;
    series_builder_series(const value_type_v& v, jdate fd, jdate ld)
      : v(v), fd(fd), ld(ld) {}
    value_type at(jdate jd) const override {
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
    if(!ordered){
      std::sort(obs.begin(), obs.end());
    }
    if(obs.empty()){
      return std::make_shared<series>();
    }
    auto fd = obs.begin()->dt;
    auto ld = obs.rbegin()->dt;
    value_type_v v;
    for(auto dt = fd; dt <= ld; dt = dt+1){
      v.push_back(default_value());
    }
    for(auto ob : obs){
      int slot = ob.dt - fd;
      v.at(slot) = ob.val;
    }
    last_build_size = obs.size();
    return std::make_shared<series_builder_series>(v,fd,ld);
  }
  
}

