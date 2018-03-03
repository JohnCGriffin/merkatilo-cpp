
#include "merkatilo.hpp"

#include <sstream>
#include <iostream>

namespace merkatilo {
  
  void dump(std::initializer_list<series_ptr> seriesz)
  {
    for(auto dt : *current_dates::active()){
      bool valid_row = false;
      std::ostringstream oss;
      oss << dt << " ";
      for (auto s : seriesz){
	oss << " ";
	oss.width(12);
	oss.precision(4);
	auto val = s->at(dt);
	if(valid(val)){
	  valid_row = true;
	  oss << std::fixed << valueof(val);
	} else {
	  oss << "      ";
	}
      }
      if(valid_row){
	std::cout << oss.str() << "\n";
      }
    }
  }

}
