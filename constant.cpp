
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr constant(double N){
    struct constant_series : public series {
      double N;
      constant_series(double N) : N(N){}
      value_type at(jdate) const override {
	return N;
      };
    };
    if(!valid(N)){
      throw std::runtime_error("constant(N) parameter was NaN");
    }
    return std::make_shared<constant_series>(N);
  }

}
