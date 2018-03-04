
#include "merkatilo.hpp"

#include <sstream>
#include <fstream>
#include <functional>

namespace merkatilo {
  
  static std::vector<std::string> split(std::string s, std::string delimiter)
  {
    std::vector<std::string> result;
    for(auto pos = s.find(delimiter); pos != std::string::npos; pos = s.find(delimiter)){
      auto token = s.substr(0,pos);
      s = s.substr(pos+1);
      if(token.size()){
	result.push_back(token);
      }
    }
    if(s.size()){
      result.push_back(s);
    }
    return result;
  }

  static series_ptr default_loader(std::string id)
  {
    std::ostringstream oss;
    oss << getenv("HOME") << "/TIME_SERIES";
    for(auto token : split(id,":")){
      oss << "/" << token;
    }
    std::string the_file = (id[0] == '/') ? id : oss.str();
    std::ifstream ifs(the_file);
    std::string date_token;
    double value;
    series_builder builder;
    std::string line;
    while(std::getline(ifs,line)){
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

  static std::string normalize_id(std::string id)
  {
    if(id[0] != '/'){
      if(id.find("::") == std::string::npos){
	return id + "::CLOSE";
      }
    }
    return id;
  }

  static thread_local std::function<series_ptr(std::string)> loader = default_loader;

  series_ptr lo(std::string id)
  {
    if(id.size() == 0){
      throw std::runtime_error("empty string given to lo.");
    }
    return loader(normalize_id(id));
  }

}
