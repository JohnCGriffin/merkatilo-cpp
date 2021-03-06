
#include "merkatilo.hpp"

#include <sstream>
#include <fstream>
#include <functional>

namespace merkatilo {
  
  static std::vector<std::string> split(std::string s, std::string delimiter)
  {
    std::vector<std::string> result;
    for(auto pos = s.find(delimiter); pos != std::string::npos; pos = s.find(delimiter)){
      const auto token = s.substr(0,pos);
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
    const std::string the_file = (id[0] == '/') ? id : oss.str();
    std::ifstream ifs(the_file);
    return read_series(ifs);
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

  using loader_t = std::function<series_ptr(std::string)>;

  static thread_local loader_t loader = default_loader;

  /**
     @brief set new series loader
  */
  void set_loader(loader_t ldr){
    loader = ldr;
  }

  /**
     @brief return current series loader.
  */
  loader_t get_loader(){
    return loader;
  }
  

  /**
     @brief load a series.
     *
     * Pass a string request in TICKER::RECORD format to a thread_local loader to fetch
     * a series.  The default loader looks for text-based series in ~/TIME_SERIES.  
     *
     * If the request is in the form of TICKER, then it is converted to TICKER::CLOSE by
     * lo before asking the loader for the series.
     */


  series_ptr lo(std::string id)
  {
    if(id.size() == 0){
      throw std::invalid_argument("empty string given to lo.");
    }
    return loader(normalize_id(id));
  }

}
