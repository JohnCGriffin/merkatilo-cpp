
#include "merkatilo.hpp"
#include <cmath>
#include <memory>


namespace merkatilo {

  void series_builder::insert(observation ob){
    auto sz = obs.size();
    if(sz && ob.dt <= obs.at(sz-1).dt){
      ordered = false;
    }
    obs.push_back(ob);
  }

  typedef std::pair<jdate_v_ptr,observations_ptr> obs_cache;
  
  struct series_builder_series : public series {
    std::unique_ptr<obs_cache> cache;
    const std::vector<std::optional<double>> v;
    jdate fd;
    jdate ld;
    series_builder_series(const std::vector<std::optional<double>>& v, jdate fd, jdate ld)
      : v(v), fd(fd), ld(ld) {}
    std::optional<double> at(jdate jd) const override {
      if(jd < fd || ld < jd){
	return {};
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
    auto fd = obs.begin()->dt;
    auto ld = obs.rbegin()->dt;
    std::vector<std::optional<double>> v;
    for(auto dt = fd; dt <= ld; dt = dt+1){
      v.push_back({});
    }
    for(auto ob : obs){
      int slot = ob.dt - fd;
      v.at(slot) = ob.val;
    }
    return std::make_shared<series_builder_series>(v,fd,ld);
  }
  
}

