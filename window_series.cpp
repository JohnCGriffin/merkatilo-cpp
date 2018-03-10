
#include "merkatilo.hpp"


namespace merkatilo {

  series_ptr window_series (series_ptr sp,
			    unsigned N,
			    std::function<double(const std::vector<double>&)> window_function,
			    bool missing_data_permitted)
  {
    auto s = sp.get();
    std::vector<double> in;
    series_builder builder;
    
    for(auto dt : *current_dates::active()){
      auto val = s->at(dt);
      in.push_back(val);
      if(!missing_data_permitted && !valid(val)){
	in.clear();
      }
      if(in.size() >= N){
	std::vector<double> section(in.begin()+(in.size()-N),in.end());
	auto result = window_function(section);
	if(valid(result)){
	  builder.insert({dt, result});
	}
      }
    }

    return builder.construct();
  }

}
