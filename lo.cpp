
#include "merkatilo.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <memory>

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

  static series default_loader(std::string id)
  {
    std::ostringstream oss;
    oss << getenv("HOME") << "/TIME_SERIES";
    for(auto token : split(id,":")){
      oss << "/" << token;
    }
    std::ifstream ifs(oss.str());
    std::string date_token;
    double value;
    series_builder builder;
    while(ifs >> date_token >> value){
      auto jd = jdate(date_token);
      builder.insert(jd,value);
    }
    return builder.construct();
  }

  static std::string normalize_id(std::string id)
  {
    if(id.find("::") == std::string::npos){
      return id + "::CLOSE";
    }
    return id;
  }

  static thread_local std::function<series(std::string)> loader = default_loader;

  series lo(std::string id)
  {
    return loader(normalize_id(id));
  }

}
