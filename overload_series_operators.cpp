
#include "merkatilo.hpp"

namespace merkatilo {

  overloaded_operator_variant::overloaded_operator_variant(series_ptr s) : s(s){}
  overloaded_operator_variant::overloaded_operator_variant(double val) : s(constant(val)){}

  class binop_series : public series {
    const series_ptr a;
    const series_ptr b;
    const std::function<double(double,double)> op;
  public:
    binop_series(series_ptr a,
		 overloaded_operator_variant b,
		 std::function<double(double,double)> op)
      : a(a), b(b.s), op(op) {}
    double at(jdate dt) const override {
      const auto a_val = a->at(dt);
      const auto b_val = b->at(dt);
      return (valid(a_val) && valid(b_val)) ? op(a_val,b_val) : default_value();
    }
  };


#define ARITHMETIC(SYM){ return std::make_shared<binop_series>	\
      (a,b,[](double v1, double v2){ return v1 SYM v2; }); }

  series_ptr operator+(series_ptr a, overloaded_operator_variant b) ARITHMETIC(+)
  series_ptr operator-(series_ptr a, overloaded_operator_variant b) ARITHMETIC(-)
  series_ptr operator*(series_ptr a, overloaded_operator_variant b) ARITHMETIC(*)
  series_ptr operator/(series_ptr a, overloaded_operator_variant b)
  {
    return std::make_shared<binop_series>
      (a,b,[](double v1, double v2){ return v2 ? (v1/v2) : default_value(); });
  }


#define INEQUALITY(SYM){ return std::make_shared<binop_series>		\
      (a,b,[](double v1, double v2){ return (v1 SYM v2) ? v1 : default_value(); }); }

  series_ptr operator< (series_ptr a, overloaded_operator_variant b) INEQUALITY(<)
  series_ptr operator<=(series_ptr a, overloaded_operator_variant b) INEQUALITY(<=)
  series_ptr operator> (series_ptr a, overloaded_operator_variant b) INEQUALITY(>)
  series_ptr operator>=(series_ptr a, overloaded_operator_variant b) INEQUALITY(>=)

  series_ptr series_or (series_ptr a, overloaded_operator_variant b){
    return std::make_shared<binop_series>(a,b,[](double v1, double v2){
	if(valid(v1)){
	  return v1;
	}
	return v2;
      });
  }
  
  series_ptr series_and (series_ptr a, overloaded_operator_variant b){
    return std::make_shared<binop_series>(a,b,[](double v1,double v2){
	if(valid(v1) && valid(v2)){
	  return v2;
	}
	return default_value();
      });
  }
  
}
