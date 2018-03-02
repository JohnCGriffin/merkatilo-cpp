
#include "merkatilo.hpp"
#include <cmath>
#include <memory>


namespace merkatilo {

  void series_builder::insert(jdate jd, double value){
    if(!std::isnan(value)){
      this->collector.insert({jd,value});
    }
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
    auto fd = collector.begin()->first;
    auto ld = collector.rbegin()->first;
    std::vector<std::optional<double>> v;
    for(auto dt = fd; dt <= ld; dt = dt+1){
      auto it = collector.find(dt);
      if(it == collector.end()){
	v.push_back({});
      } else {
	v.push_back(it->second);
      }
    }
    return std::make_shared<series_builder_series>(v,fd,ld);
  }
  
}

