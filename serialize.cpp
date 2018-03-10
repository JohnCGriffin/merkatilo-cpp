
#include "merkatilo.hpp"

namespace merkatilo {
  
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
