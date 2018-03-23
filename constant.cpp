
#include "merkatilo.hpp"


namespace merkatilo {

/**
@brief return a constant value.
*/

  series_ptr constant(double N){
    struct constant_series : public series {
      double N;
      constant_series(double N) : N(N){}
      double at(jdate) const override {
	return N;
      };
    };
    if(!valid(N)){
      throw std::invalid_argument("constant(N) parameter was NaN");
    }
    return std::make_shared<constant_series>(N);
  }

}
