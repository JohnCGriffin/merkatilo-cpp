
#include "merkatilo.hpp"

namespace merkatilo {

  /// Each input series takes part in a vector<double> set of values which the supplied
  /// transformation function f converts to a single scalar on each date.  If missing_data_permitted
  /// is false then only dates where every series has a value will call the function.  When true,
  /// the function must handle NaN inputs.

  series_ptr series_map (std::vector<series_ptr> sps,
			 std::function<double(const std::vector<double>&)> f,
			 bool missing_data_permitted,
			 dateset_ptr dates)
  {
    if(!sps.size()){
      throw std::invalid_argument("series_map requires at least one input series");
    }
    // faster native series pointers
    std::vector<series*> ss;
    for(const auto sp : sps){
      ss.push_back(sp.get());
    }

    series_builder builder;

    for(const auto dt : *dates){
      
      std::vector<double> v;

      for(const auto s : ss){
	const auto val = s->at(dt);
	if(!missing_data_permitted && !valid(val)){
	  break;
	}
	v.push_back(val);
      }
      
      if(v.size() == ss.size()){
	const auto result = f(v);
	if(valid(result)){
	  builder.insert ({dt, result });
	}
      }
    }

    return builder.construct();
  }

}
