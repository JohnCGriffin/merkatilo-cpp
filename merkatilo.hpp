
#ifndef _MERKATILO_HPP_293487293847_
#define _MERKATILO_HPP_293487293847_

#include <set>
#include <string>
#include <iostream>
#include <tuple>
#include <functional>
#include <memory>
#include <initializer_list>

namespace merkatilo {

  
  class jdate {
    jdate(unsigned jd);
    unsigned jd;
  public:
    jdate(int y, int m, int d);
    jdate(std::string s);
    jdate(const jdate& another);
    std::string to_string(void) const;
    std::tuple<int,int,int> ymd(void) const;
    int year() const;
    int month() const;
    int day() const;
    jdate operator+(int offset) const;
    jdate operator-(int offset) const;
    int operator-(const jdate& another) const;
    bool operator<(const jdate& another) const;
    bool operator<=(const jdate& another) const;
    unsigned julian() const;
  
    static const jdate MAX_DATE;
    static const jdate MIN_DATE;

    friend std::ostream& operator<<(std::ostream&, const jdate& jd);
  };

  jdate today(void);

  typedef std::function<std::optional<double>(jdate)> series;

  struct series_builder {
    std::unordered_map<unsigned,double> collector;
    void insert(jdate,double);
    series construct();
  };

  typedef const std::set<jdate> dateset;

  class dateset_builder {
    std::set<jdate> _so_far;
    std::optional<jdate> _low;
    std::optional<jdate> _high;
  public:
    dateset_builder(dateset&);
    dateset_builder(series&);
    dateset_builder(jdate low, jdate high);
    dateset_builder& intersect(dateset&);
    dateset_builder& intersect(series&);
    dateset_builder& add(dateset&);
    dateset_builder& add(series&);
    dateset_builder& low_date(jdate);
    dateset_builder& high_date(jdate);
    dateset construct();
  };

  class current_dates {
    std::shared_ptr<dateset> previous;
  public:
    current_dates(series& s);
    current_dates(dateset& ds);
    ~current_dates();
    static const dateset& active();
  };

  series lo(std::string id);
  void dump(std::initializer_list<series>);

  series ema(series, unsigned N);
  series sma(series, unsigned period);

}

#endif

