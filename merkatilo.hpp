
#ifndef _MERKATILO_HPP_293487293847_
#define _MERKATILO_HPP_293487293847_

#include <set>
#include <string>
#include <iostream>
#include <tuple>
#include <functional>
#include <memory>
#include <initializer_list>
#include <map>

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

  class series {
  public:
    virtual ~series();
    virtual std::optional<double> at(jdate) const;
  };

  typedef std::shared_ptr<series> series_ptr;

  struct observation {
    jdate dt;
    double val;
    bool operator<(const observation& another){
      return dt.julian() < another.dt.julian();
    }
  };
  
  struct series_builder {
    bool ordered = true;
    std::vector<observation> obs;
    void insert(observation);
    series_ptr construct();
  };

  typedef const std::vector<jdate> dateset;
  typedef std::shared_ptr<dateset> dateset_ptr;

  class dateset_builder {
    std::set<jdate> _so_far;
    std::optional<jdate> _low;
    std::optional<jdate> _high;
  public:
    dateset_builder(dateset_ptr);
    dateset_builder(series_ptr);
    dateset_builder(jdate low, jdate high);
    dateset_builder& intersect(dateset_ptr);
    dateset_builder& intersect(series_ptr);
    dateset_builder& add(dateset_ptr);
    dateset_builder& add(series_ptr);
    dateset_builder& low_date(jdate);
    dateset_builder& high_date(jdate);
    dateset construct();
  };

  class current_dates {
    std::shared_ptr<dateset> previous;
  public:
    current_dates(series_ptr s);
    current_dates(dateset_ptr ds);
    ~current_dates();
    static const dateset& active();
  };

  series_ptr lo(std::string id);
  void dump(std::initializer_list<series_ptr>);

  series_ptr ema(series_ptr, unsigned N);
  series_ptr sma(series_ptr, unsigned period);

}

#endif

