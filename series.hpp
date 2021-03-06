
#ifndef _SERIES_HPP_20394872039487_
#define _SERIES_HPP_20394872039487_

/** @file */

#include <vector>
#include <memory>
#include <limits>

#include "jdate.hpp"

namespace merkatilo {

  inline bool valid(double v){ return v == v; }
  inline double default_value() { return std::numeric_limits<double>::quiet_NaN(); }

  typedef std::vector<double> double_v;
  typedef std::shared_ptr<double_v> double_v_ptr;

  /// @brief date/value pair
  struct observation {
    jdate dt;
    double val;
    bool operator<(const observation& another) const {
      return dt < another.dt;
    }
  };

  typedef std::pair<observation,observation> obpair;
  typedef std::vector<observation> observations;
  typedef std::shared_ptr<const observations> observations_ptr;

  /// @brief date-to-value mapper
  class series {
  public:
    virtual ~series();
    /// get the double value at a date
    virtual double at(jdate) const;
    /// map given dates to values
    virtual observations_ptr observations_by_date (jdate_v_ptr dates);
  };

  /// @brief all series are passed by shared_ptr
  typedef std::shared_ptr<series> series_ptr;

  /// @brief mutable builder of immutable series_ptr
  struct series_builder {
    bool ordered = true;
    std::vector<observation> obs;
    series_builder& insert(observation);
    series_ptr construct();
  };



}

#endif

