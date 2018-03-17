
#include "merkatilo.hpp"

namespace merkatilo {

/**
 @brief read series from text strem
 * Given an input stream with lines containing 
 * two space-separated fields in the form 
 * YYYY-MM-DD DOUBLE, convert into a series.
 * The input can have empty lines and comment
 * lines beginning with #.
 */
  
  series_ptr read_series(std::istream& is)
  {
    std::string date_token;
    double value;
    series_builder builder;
    std::string line;
    while(std::getline(is,line)){
      if(line.find("#") == 0){
	continue;
      }
      std::istringstream iss(line);
      if(iss >> date_token >> value){
	auto jd = parse_jdate(date_token);
	builder.insert({jd,value});
      }
    }
    return builder.construct();
  }

  /**
   @brief write series to output stream
   * output series, one line per observation,
   * each line containing two fields, a YYYY-MM-DD
   * date and a double.
   */

  void write_series(series_ptr sp, dateset_ptr dts, std::ostream& os)
  {
    auto s = sp.get();
    for(auto dt : *dts){
      auto val = s->at(dt);
      if(valid(val)){
	os << jdate_to_string(dt) << " " << val << "\n";
      }
    }
  }

}
