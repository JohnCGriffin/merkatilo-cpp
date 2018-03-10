
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr series_map (std::vector<series_ptr> sps,
			 std::function<double(const std::vector<double>&)> f,
			 bool missing_data_permitted)
  {
    if(!sps.size()){
      throw std::runtime_error("series_map requires at least one input series");
    }
    // faster native series pointers
    std::vector<series*> ss;
    for(auto sp : sps){
      ss.push_back(sp.get());
    }

    series_builder builder;

    for(auto dt : *current_dates::active()){
      
      std::vector<double> v;

      for(auto s : ss){
	auto val = s->at(dt);
	if(!missing_data_permitted && !valid(val)){
	  break;
	}
	v.push_back(val);
      }
      
      if(v.size() == ss.size()){
	auto result = f(v);
	if(valid(result)){
	  builder.insert ({dt, result });
	}
      }
    }

    return builder.construct();
  }

}
