
#ifndef _JDATES_HPP_203948762039847_
#define _JDATES_HPP_203948762039847_

/** @file */

#include <vector>
#include <memory>

namespace merkatilo {

  typedef unsigned jdate;

  
  /// @brief convert Y,M,D to julian integer
  /// @param y year
  /// @param m month
  /// @param d day of month
  /// @return integer representing julian day
  
  jdate ymd_to_jdate(int y,int m, int d);


  /// @brief convert text to julian date
  /// @param text "YYYY-MM-DD" -> julian integer
  
  jdate parse_jdate(std::string text);


  /// @brief convert julian date into year/month/day triple
  /// @param jd julian date
  /// @return year,month,day triple
  
  std::tuple<int,int,int> ymd(jdate jd);


  /// @brief year of julian integer
  /// @param jd julian integer
  /// @return year
  
  int year(jdate jd);


  /// @brief month of julian integer
  /// @param jd julian integer
  /// @return month (where 1 = January)
  
  int month(jdate jd);


  /// @brief day of julian integer
  /// @param jd julian integer
  /// @return day of month (1-31)
  
  int day(jdate jd);


  /// @brief convert julian integer to string
  /// @param jd julian date
  /// @return string in "YYYY-MM-DD" form

  std::string jdate_to_string(jdate jd);


  /// @brief return julian date for today
  jdate today(void);


  extern const jdate MAX_DATE, MIN_DATE;

  typedef std::vector<jdate> jdate_v;
  typedef std::shared_ptr<const jdate_v> jdate_v_ptr;

  
}

#endif
