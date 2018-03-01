
#include "merkatilo.hpp"
#include <cmath>
#include <memory>


namespace merkatilo {

  void series_builder::insert(jdate jd, double value){
    if(!std::isnan(value)){
      this->collector.insert({jd.julian(),value});
    }
  }

  struct series_builder_series {
    using mtype_t = std::unordered_map<unsigned,double>;
    using mptype_t = std::shared_ptr<mtype_t>;
    mptype_t sp;
    series_builder_series(mptype_t sp):sp(sp){}
    std::optional<double> operator()(jdate jd) const {
      auto p = sp->find(jd.julian());
      if(p == sp->end()){
	return {};
      }
      return p->second;
    }
  };

  series series_builder::construct()
  {
    auto p = std::make_shared<series_builder_series::mtype_t>(this->collector);
    return series_builder_series(p);
  }
  
}

