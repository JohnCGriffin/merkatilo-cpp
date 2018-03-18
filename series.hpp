
#ifndef _SERIES_HPP_20394872039487_
#define _SERIES_HPP_20394872039487_

#include <vector>
#include <memory>
#include <limits>

#include "jdate.hpp"

namespace merkatilo {

  typedef double value_type;
  inline bool valid(value_type v){ return v == v; }
  inline value_type default_value() { return std::numeric_limits<double>::quiet_NaN(); }

  typedef std::vector<value_type> value_type_v;
  typedef std::shared_ptr<value_type_v> value_type_v_ptr;

  struct observation {
    jdate dt;
    value_type val;
    bool operator<(const observation& another) const {
      return dt < another.dt;
    }
  };

  typedef std::pair<observation,observation> obpair;
  typedef std::vector<observation> observations;
  typedef std::shared_ptr<const observations> observations_ptr;

  class series {
  public:
    virtual ~series();
    /// get the double value at a date
    virtual value_type at(jdate) const;
    /// map given dates to values
    virtual observations_ptr observations_by_date (jdate_v_ptr dates);
  };

  typedef std::shared_ptr<series> series_ptr;

  struct series_builder {
    bool ordered = true;
    std::vector<observation> obs;
    void insert(observation);
    series_ptr construct();
  };



}

#endif

