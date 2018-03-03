
#include "merkatilo.hpp"

#include <stdexcept>
#include <ctime>

namespace merkatilo {
  
  jdate ymd_to_jdate(const int year, const int month, const int day)
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

  jdate parse_jdate(std::string s){
    int y,m,d;
    if(sscanf(s.c_str(),"%d-%d-%d", &y,&m,&d) != 3){
      throw std::invalid_argument("invalid date string: " + s);
    }
    return ymd_to_jdate(y,m,d);
  }

  std::tuple<int,int,int> ymd(jdate JD) {
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

  int year(jdate jd){
    return std::get<0>(ymd(jd));
  }

  int month(jdate jd){
    return std::get<1>(ymd(jd));
  }

  int day(jdate jd){
    return std::get<2>(ymd(jd));
  }

  std::string jdate_to_string(jdate jd) {
    auto tmp = ymd(jd);
    char buf[32];
    sprintf(buf, "%04d-%02d-%02d", std::get<0>(tmp), std::get<1>(tmp), std::get<2>(tmp));
    return std::string(buf);
  }


  jdate today(void) {
    auto t = std::time(nullptr);
    std::tm* tmstr = localtime(&t);
    return ymd_to_jdate(tmstr->tm_year + 1900, tmstr->tm_mon+1, tmstr->tm_mday);
  }


  const jdate MIN_DATE = ymd_to_jdate(1700,1,1);
  const jdate MAX_DATE = ymd_to_jdate(2100,12,31);

}
