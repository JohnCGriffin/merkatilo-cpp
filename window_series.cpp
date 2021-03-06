
#include "merkatilo.hpp"


namespace merkatilo {

  /// Each date which terminates N observations is the result of 
  /// passing a vector<double> of those observations to a supplied
  /// window_function.  For instance, given an average function,
  /// the result of using it as a window function will be identical 
  /// to calling 'ma'.

  series_ptr window_series (series_ptr sp,
			    unsigned N,
			    std::function<double(const std::vector<double>&)> window_function,
			    bool missing_data_permitted,
			    dateset_ptr dates)
  {
    const auto s = sp.get();
    std::vector<double> in;
    series_builder builder;
    
    for(const auto dt : *dates){
      const auto val = s->at(dt);
      in.push_back(val);
      if(!missing_data_permitted && !valid(val)){
	in.clear();
      }
      if(in.size() >= N){
	const std::vector<double> section(in.begin()+(in.size()-N),in.end());
	const auto result = window_function(section);
	if(valid(result)){
	  builder.insert({dt, result});
	}
      }
    }

    return builder.construct();
  }

}
