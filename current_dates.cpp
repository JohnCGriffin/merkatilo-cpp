
#include "merkatilo.hpp"
#include <exception>

namespace merkatilo {

  static thread_local dateset_ptr _current_dateset;

  current_dates::current_dates(series_ptr s){
    this->previous = _current_dateset;
    _current_dateset = std::make_shared<dateset>(dateset_builder(s).construct());
  }

  current_dates::current_dates(dateset_ptr ds){
    this->previous = _current_dateset;
    _current_dateset = std::make_shared<dateset>(dateset_builder(ds).construct());
  }

  current_dates::~current_dates(){
    _current_dateset = this->previous;
  }

  const dateset& current_dates::active() {
    if(!_current_dateset.get()){
      throw std::logic_error("current_dates not yet set");
    }
    return *_current_dateset;
  }
  
}
