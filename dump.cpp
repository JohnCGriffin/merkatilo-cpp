
#include "merkatilo.hpp"

#include <sstream>
#include <iostream>

namespace merkatilo {

  /**
     @brief display multiple series in spreadsheet style.
  */
  
  void dump(std::initializer_list<series_ptr> seriesz)
  {
    for(const auto dt : *current_dates::active()){
      bool valid_row = false;
      std::ostringstream oss;
      oss << jdate_to_string(dt) << " ";
      for (const auto s : seriesz){
	oss << " ";
	oss.width(12);
	oss.precision(4);
	const auto val = s->at(dt);
	if(valid(val)){
	  valid_row = true;
	  oss << std::fixed << val;
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
