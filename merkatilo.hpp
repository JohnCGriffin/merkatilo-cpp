
#ifndef _MERKATILO_HPP_293487293847_
#define _MERKATILO_HPP_293487293847_

#include <set>
#include <string>
#include <memory>
#include <limits>
#include <vector>

namespace merkatilo {

  class opt_double {
    double _d;
  public:
    inline opt_double() : _d(std::numeric_limits<double>::quiet_NaN()) {}
    inline opt_double(double v) : _d(v) {}
    inline operator bool() const { return _d == _d; }
    inline double operator*() const { return _d; }
    inline double value() const { return _d; }
  };

  typedef std::vector<opt_double> opt_double_v;
  typedef std::shared_ptr<opt_double_v> opt_double_v_ptr;

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

  struct observation {
    jdate dt;
    opt_double val;
    bool operator<(const observation& another) const {
      return dt < another.dt;
    }
  };

  typedef std::vector<observation> observations;
  typedef std::shared_ptr<const observations> observations_ptr;

  class series {
  public:
    virtual ~series();
    virtual opt_double at(jdate) const;
    virtual observations_ptr observations_by_date (jdate_v_ptr dates);
  };

  typedef std::shared_ptr<series> series_ptr;

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
    jdate _low;
    jdate _high;
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
    static const dateset_ptr active();
  };

  series_ptr lo(std::string id);
  void dump(std::initializer_list<series_ptr>);

  series_ptr ema(series_ptr, unsigned N);
  series_ptr sma(series_ptr, unsigned period);

}

#endif

