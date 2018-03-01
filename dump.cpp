
#include "merkatilo.hpp"

#include <sstream>

namespace merkatilo {
  
  void dump(std::initializer_list<series> seriesz)
  {
    for(auto dt : current_dates::active()){
      bool valid_row = false;
      std::ostringstream oss;
      oss << dt << " ";
      for (auto s : seriesz){
	oss << " ";
	oss.width(12);
	oss.precision(4);
	auto val = s(dt);
	if(val){
	  valid_row = true;
	  oss << std::fixed << val.value();
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
