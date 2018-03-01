
#include "merkatilo.hpp"

#include <stdexcept>
#include <ctime>

namespace merkatilo {
  
  static unsigned to_jd(const int year, const int month, const int day)
  {
    auto is_leap_year = [](int y){
      if(y % 4 == 0){
	if (y % 400 == 0){
	  return true;
	}
	return (y % 100 != 0);
      }
      return false;
    };

    bool month_day_ok = false;
    switch(month){
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      month_day_ok = (0 < day) && (day < 32);
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      month_day_ok = (0 < day) && (day < 31);
      break;
    case 2:
      if(day == 29){
	month_day_ok = is_leap_year(year);
      } else {
	month_day_ok = (0 < day) && (day < 29);
      }
      break;
    }

    if(!month_day_ok){
      throw std::invalid_argument("invalid month");
    }
  
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = (month + (12 * a) -3);

    return day
      + (((153 * m) + 2) / 5)
      + (365 * y)
      + (y / 4)
      - (y / 100)
      + (y / 400)
      - 32045;
  }

  static int to_jd(std::string s){
    int y,m,d;
    if(sscanf(s.c_str(),"%d-%d-%d", &y,&m,&d) != 3){
      throw std::invalid_argument("invalid date string: " + s);
    }
    return to_jd(y,m,d);
  }

  jdate::jdate(int y, int m, int d) : jd(to_jd(y,m,d)){}

  jdate::jdate(std::string s):jd(to_jd(s)){}

  jdate::jdate(const jdate& another) : jd(another.jd){}

  jdate::jdate(unsigned jd) : jd(jd){}

  std::tuple<int,int,int> jdate::ymd() const {
    int JD = this->jd;
    int L = JD + 68569;
    int N = (4 * L) / 146097;

    L = L - (((146097 * N) + 3) / 4);
    int I = (4000 * (L+1)) /  1461001;
    L = L - ((1461 * I) / 4) + 31;
    int J = (80 * L) / 2447;
    int K = L - ((2447 * J) / 80);
    L = J / 11;
    J = J + 2 + (-12 * L);
    I = (100 * (N - 49)) + I + L;
    return {I,J,K};
  }

  int jdate::year(void) const {
    return std::get<0>(this->ymd());
  }

  int jdate::month(void) const {
    return std::get<1>(this->ymd());
  }

  int jdate::day(void) const {
    return std::get<2>(this->ymd());
  }

  jdate jdate::operator+(int offset) const {
    return jdate(this->jd+offset);
  }

  jdate jdate::operator-(int offset) const {
    return jdate(this->jd-offset);
  }

  int jdate::operator-(const jdate& another) const {
    int jd = this->jd;
    return jd - another.jd;
  }

  bool jdate::operator<(const jdate& another) const {
    return this->jd < another.jd;
  }

  bool jdate::operator<=(const jdate& another) const {
    return this->jd <= another.jd;
  }

  std::string jdate::to_string() const {
    auto ymd = this->ymd();
    char buf[32];
    sprintf(buf, "%04d-%02d-%02d", std::get<0>(ymd), std::get<1>(ymd), std::get<2>(ymd));
    return std::string(buf);
  }

  std::ostream& operator<<(std::ostream& os, const jdate& jd){
    return os << jd.to_string();
  }

  jdate today(void) {
    auto t = std::time(nullptr);
    std::tm* tmstr = localtime(&t);
    return jdate(tmstr->tm_year + 1900, tmstr->tm_mon+1, tmstr->tm_mday);
  }

  unsigned jdate::julian() const {
    return this->jd;
  }

  const jdate jdate::MIN_DATE = jdate(1700,1,1);
  const jdate jdate::MAX_DATE = jdate(2100,12,31);

}
