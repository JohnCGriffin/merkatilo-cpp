
#include "merkatilo.hpp"

namespace merkatilo {

  class binop_series : public series {
    series_ptr a;
    series_ptr b;
    std::function<double(double,double)> op;
  public:
    binop_series(series_ptr a, series_ptr b, std::function<double(double,double)> op)
      : a(a), b(b), op(op) {}
    binop_series(series_ptr a, double b_val, std::function<double(double,double)> op)
      : a(a), b(constant(b_val)), op(op) {}
    double at(jdate dt) const override {
      auto a_val = a->at(dt);
      auto b_val = b->at(dt);
      return (valid(a_val) && valid(b_val)) ? op(a_val,b_val) : default_value();
    }
  };


#define ARITHMETIC(SYM){ return std::make_shared<binop_series>\
      (a,b,[](double v1, double v2){ return v1 SYM v2; }); }

  series_ptr operator+(series_ptr a, series_ptr b) ARITHMETIC(+)
  series_ptr operator+(series_ptr a, double b) ARITHMETIC(+)

  series_ptr operator-(series_ptr a, series_ptr b) ARITHMETIC(-)
  series_ptr operator-(series_ptr a, double b) ARITHMETIC(-)

  series_ptr operator*(series_ptr a, series_ptr b) ARITHMETIC(*)
  series_ptr operator*(series_ptr a, double b) ARITHMETIC(*)

  series_ptr operator/(series_ptr a, series_ptr b)
  {
    return std::make_shared<binop_series>
      (a,b,[](double v1, double v2){ return v2 ? (v1/v2) : default_value(); });
  }

  series_ptr operator/(series_ptr a, double b)
  {
    return std::make_shared<binop_series>
      (a,b,[](double v1, double v2){ return v2 ? (v1/v2) : default_value(); });
  }

#define INEQUALITY(SYM){ return std::make_shared<binop_series>\
			 (a,b,[](double v1, double v2){ return (v1 SYM v2) ? v1 : default_value(); }); }

  series_ptr operator<(series_ptr a,series_ptr b) INEQUALITY(<)
  series_ptr operator<(series_ptr a,double b) INEQUALITY(<)

  series_ptr operator<=(series_ptr a,series_ptr b) INEQUALITY(<=)
  series_ptr operator<=(series_ptr a,double b) INEQUALITY(<=)

  series_ptr operator>(series_ptr a,series_ptr b) INEQUALITY(>)
  series_ptr operator>(series_ptr a,double b) INEQUALITY(>)

  series_ptr operator>=(series_ptr a,series_ptr b) INEQUALITY(>=)
  series_ptr operator>=(series_ptr a,double b) INEQUALITY(>=)


  series_ptr series_or (series_ptr a, series_ptr b){
    return std::make_shared<binop_series>(a,b,[](double v1, double v2){
	if(valid(v1)){
	  return v1;
	}
	return v2;
      });
  }
  
  series_ptr series_or (series_ptr a, double b) {
    return series_or(a, constant(b));
  }

  series_ptr series_and (series_ptr a, series_ptr b){
    return std::make_shared<binop_series>(a,b,[](double v1,double v2){
	if(valid(v1) && valid(v2)){
	  return v2;
	}
	return default_value();
      });
  }
  
  series_ptr series_and (series_ptr a, double b){
    return series_and(a, constant(b));
  }


}
