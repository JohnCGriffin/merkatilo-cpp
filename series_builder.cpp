
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

  struct series_builder_series : public series {
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
  };

  std::shared_ptr<series> series_builder::construct()
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

