
#ifndef _JDATES_HPP_203948762039847_
#define _JDATES_HPP_203948762039847_

#include <vector>
#include <memory>

namespace merkatilo {

  typedef unsigned jdate;

  jdate ymd_to_jdate(int y,int m, int d);
  jdate parse_jdate(std::string);
  std::tuple<int,int,int> ymd(jdate);
  int year(jdate);
  int month(jdate);
  int day(jdate);
  std::string jdate_to_string(jdate);

  jdate today(void);

  extern const jdate MAX_DATE, MIN_DATE;

  typedef std::vector<jdate> jdate_v;
  typedef std::shared_ptr<const jdate_v> jdate_v_ptr;

  
}

#endif
