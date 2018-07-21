

#include "catch.hpp"

#include "merkatilo.hpp"
#include <sstream>
#include <exception>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <memory>

using namespace merkatilo;


static bool approximates(double a, double b, double epsilon=0.00001){
  return (std::abs(a-b) < epsilon);
}

static bool approximates (const std::vector<double>& a, const std::vector<double>& b)
{
  if(a.size() != b.size()){
    return false;
  }
  for(size_t i=0; i<a.size(); i++){
    if(!(approximates(a.at(i),b.at(i)))){
      return false;
    }
  }
  return true;
}

static bool verify_equivalency(series_ptr a, series_ptr b)
{
  for(const auto dt : *current_dates::active()){
    if(valid(a->at(dt)) != valid(b->at(dt))){
      if(valid(a->at(dt))){
	throw std::logic_error(("second series missing data at ")
			       + jdate_to_string(dt)
			       + " when first has "
			       + std::to_string(a->at(dt)));
      } else {
	throw std::logic_error(("first series missing data at ")
			       + jdate_to_string(dt)
			       + " when second has "
			       + std::to_string(b->at(dt)));
      }
    }
    const auto a_val = a->at(dt);
    if(!valid(a_val)){
      continue;
    }
    const auto b_val = b->at(dt);
    if(!approximates(a_val,b_val)){
      std::ostringstream oss;
      oss << "mismatched series at " << jdate_to_string(dt)
	  << " " << a_val << " vs. " << b_val;
      throw std::logic_error(oss.str());
    }
  }
  return true;
}

static series_ptr test_lo (std::string name){
  return lo("/tmp/merkatilo-test-data/" + name + ".txt");
}

static auto TEST_SERIES = test_lo("test-series");
static auto AAA_SERIES = test_lo("aaa");
static auto BBB_SERIES = test_lo("bbb");

TEST_CASE("Test that load worked"){
  current_dates active(TEST_SERIES);
  REQUIRE(series_count(TEST_SERIES) == 754);
}

TEST_CASE("EMA-FRACTIONAL"){

  const auto EMA_3_SERIES = test_lo("ema-3");
  current_dates active(TEST_SERIES);

  SECTION("ema generates known correct values"){
    REQUIRE(verify_equivalency(ema(TEST_SERIES,3), EMA_3_SERIES));
  }

  SECTION("ema is same as fractional"){
    REQUIRE(verify_equivalency(ema(TEST_SERIES,3), fractional(TEST_SERIES,0.5)));
  }

  SECTION("ema length is equivalent to input"){
    REQUIRE(series_count(TEST_SERIES) == series_count(ema(TEST_SERIES,20)));
  }

  SECTION("handling bad argument"){
    REQUIRE_THROWS(ema(TEST_SERIES,1));
  }
}

TEST_CASE("MA"){

  auto MA_3_SERIES = test_lo("ma-3");
  current_dates active(TEST_SERIES);

  SECTION("ma generates known correct values"){
    REQUIRE(verify_equivalency(ma(TEST_SERIES,3), MA_3_SERIES));
  }

  SECTION("ma generate len(input)-N+1 output values"){
    unsigned N = 10;
    REQUIRE(series_count(ma(TEST_SERIES,N)) == series_count(TEST_SERIES)-N+1);
  }

  SECTION("handling bad period argument"){
    REQUIRE_THROWS(ma(TEST_SERIES,1));
  }
  
}

TEST_CASE("MIN-MAX"){

  current_dates active(TEST_SERIES);
  const auto mm = min_max_obs(TEST_SERIES);

  SECTION("check max observation"){
    REQUIRE(jdate_to_string(mm.second.dt) == "2014-09-18");
  }

  SECTION("check min observation"){
    REQUIRE(jdate_to_string(mm.first.dt) == "2012-01-13");
  }

}

TEST_CASE("FIRST-LAST"){

  dateset_builder builder(parse_jdate("2010-1-1"),today());
  const auto dts = std::make_shared<dateset>(builder.construct());
  current_dates active(dts);

  SECTION("check first observation"){
    REQUIRE(jdate_to_string(first_ob(TEST_SERIES).dt) == "2012-01-03");
  }

  SECTION("check last observation"){
    REQUIRE(jdate_to_string(last_ob(TEST_SERIES).dt) == "2014-12-31");
  }

  SECTION("check first and last dates"){
    REQUIRE(first_date() == dts->at(0));
    REQUIRE(first_date(dts) == dts->at(0));
    REQUIRE(last_date() == dts->at(dts->size()-1));
    REQUIRE(last_date() == last_date(dts));
  }
  
}

TEST_CASE("MOMENTUM"){

  const auto MO_3_SERIES = test_lo("mo-3");
  current_dates active(TEST_SERIES);

  SECTION("mo generates known correct values"){
    REQUIRE(verify_equivalency(mo(TEST_SERIES,3), MO_3_SERIES));
  }

  SECTION("ma generate len(input)-N output values"){
    unsigned N = 10;
    REQUIRE(series_count(mo(TEST_SERIES,N)) == series_count(TEST_SERIES)-N);
  }

  SECTION("handling bad period argument"){
    REQUIRE_THROWS(mo(TEST_SERIES,0));
  }

  SECTION("mo_days generates known correct values"){
    REQUIRE(verify_equivalency(mo_days(TEST_SERIES,200), test_lo("mo-days-200")));
  }
}

TEST_CASE("TO-SIGNALS"){

  current_dates active(TEST_SERIES);

  SECTION("to_signals generates known correct values"){
    const auto sigs = to_signals(mo(TEST_SERIES,240));
    const auto obs = observations {
      { parse_jdate("2012-12-17"), 1 },
      { parse_jdate("2013-08-30"), -1 },
      { parse_jdate("2013-9-3"), 1},
      { parse_jdate("2014-12-12"), -1 },
      { parse_jdate("2014-12-19"), 1}
    };
    const auto literal = obs_to_series(std::make_shared<observations>(obs));
    REQUIRE(verify_equivalency(literal,sigs));
  }

}

TEST_CASE("DRAWDOWN"){

  current_dates active(TEST_SERIES);

  SECTION("drawdown generates known correct values"){
    const auto dd = series_drawdown(TEST_SERIES);
    REQUIRE(jdate_to_string(dd.max.dt) == "2014-09-18");
    REQUIRE(jdate_to_string(dd.min.dt) == "2014-12-16");
  }

  SECTION("drawdowns"){
    const auto dds = series_drawdowns(TEST_SERIES, .92);
    std::vector<double> vals;
    std::transform(dds.begin(), dds.end(), std::back_inserter(vals), [](const drawdown& dd){ return dd.residual(); });
    std::vector<double> test_values { 0.889196675900277, 0.9038461538461539, 0.9046052631578947 };
    REQUIRE(approximates(vals, test_values));
  }

}

TEST_CASE("VOLATILITY"){

  current_dates active(TEST_SERIES);

  REQUIRE(approximates(0.038702419488645126, volatility(TEST_SERIES)));
  REQUIRE(approximates(0.0468086666214253, volatility(TEST_SERIES, 200)));
  
}

TEST_CASE("UNREPEATED"){

  current_dates active(TEST_SERIES);

  REQUIRE(series_count(TEST_SERIES) > series_count(unrepeated(TEST_SERIES)));
  REQUIRE(verify_equivalency(unrepeated(TEST_SERIES),unrepeated(unrepeated(TEST_SERIES))));
}


TEST_CASE("REPEATED"){

  current_dates active(TEST_SERIES);

  const auto unrptd = unrepeated(TEST_SERIES);

  REQUIRE(series_count(unrptd) < series_count(TEST_SERIES));
  REQUIRE(verify_equivalency(repeated(unrptd, true),TEST_SERIES));
  REQUIRE(verify_equivalency(TEST_SERIES,repeated(TEST_SERIES)));
  
}


TEST_CASE("WARP"){

  current_dates active(TEST_SERIES);

  const auto wa1 = warp(TEST_SERIES,1);
  const auto back_again = warp(wa1, -1);

  REQUIRE(series_count(TEST_SERIES) == series_count(back_again)+1);

  current_dates active2(back_again);

  REQUIRE(verify_equivalency(TEST_SERIES,back_again));

}

TEST_CASE("CROSS"){

  current_dates active(TEST_SERIES);
  const auto CROSS_EMA_30 = test_lo("cross-ema-30");

  SECTION("cross generates known correct values"){
    REQUIRE(verify_equivalency(cross(ema(TEST_SERIES,30), TEST_SERIES), CROSS_EMA_30));
  }

  SECTION("cross default valued parameters"){
    REQUIRE(verify_equivalency(cross(ema(TEST_SERIES,30),TEST_SERIES),
			       cross(ema(TEST_SERIES,30),TEST_SERIES,1.0,1.0)));
  }

  SECTION("cross checks parameter validity"){
    REQUIRE_THROWS(cross(TEST_SERIES,TEST_SERIES,2.0,1.0));
    REQUIRE_THROWS(cross(TEST_SERIES,TEST_SERIES,1.0,2.0));
    REQUIRE_THROWS(cross(TEST_SERIES,TEST_SERIES,.2,1.0));
    REQUIRE_THROWS(cross(TEST_SERIES,TEST_SERIES,1.0,.2));
  }
  
}

TEST_CASE("CONSTANT"){

  const auto test_date = parse_jdate("2000-1-1");
  
  REQUIRE(constant(123.456)->at(test_date) == 123.456);
  REQUIRE_THROWS(constant(default_value()));
}

TEST_CASE("EQUITYLINE"){

  current_dates active(TEST_SERIES);
  const auto EQUITYLINE_EMA_10 = test_lo("equityline-ema-10");
  const auto crossed = cross(ema(TEST_SERIES,10), TEST_SERIES);
  REQUIRE(verify_equivalency(equity_line(TEST_SERIES,crossed),
			     EQUITYLINE_EMA_10));

}


TEST_CASE("REVERSE"){

  current_dates active(TEST_SERIES);
  const auto REVERSALS_95_105 = test_lo("reversals-95-105");
  const auto REVERSALS_91_109 = test_lo("reversals-91-109");

  REQUIRE(verify_equivalency(reversals(TEST_SERIES,0.95,1.05),
			     REVERSALS_95_105));

  REQUIRE(verify_equivalency(reversals(TEST_SERIES,0.91,1.09),
			     REVERSALS_91_109));
  
}

TEST_CASE("PERFORMANCE"){

  current_dates active(TEST_SERIES);
  const auto magical_sigs = nostradamus(TEST_SERIES, .97, 1.03);
  const auto magical_performance = investment_performance(TEST_SERIES, magical_sigs);

  REQUIRE(approximates(gpa(TEST_SERIES), 0.07688365986138823));

  REQUIRE(approximates(0.49379739499778585454, magical_performance.annualized_gain));
  REQUIRE(approximates(0.97126436781609182258, magical_performance.drawdown_residual));
  REQUIRE(approximates(0.92783565627193753667, magical_performance.volatility_residual));
  REQUIRE(approximates(0.67828418230563, magical_performance.long_ratio));
  REQUIRE(approximates(31,magical_performance.trades));
  
}

TEST_CASE("CONVICTION"){

  current_dates active(TEST_SERIES);
  const auto MO_5_CONVICTION_4 = test_lo("mo-5-conviction-4");

  REQUIRE(verify_equivalency(conviction(mo(TEST_SERIES,5),4),
			     MO_5_CONVICTION_4));
			     
}


TEST_CASE("FILTER"){

  current_dates active(TEST_SERIES);
  REQUIRE(2 == series_count(filter(TEST_SERIES, [](double val){ return val > 360;})));
}


TEST_CASE("ARITHMETIC OPERATORS"){

  current_dates active(TEST_SERIES);
  const auto doubled = TEST_SERIES + TEST_SERIES;
  const auto times_two = TEST_SERIES * 2;
  const auto fo = first_ob(TEST_SERIES);

  REQUIRE(TEST_SERIES->at(fo.dt) * 2 == doubled->at(fo.dt));
  REQUIRE(doubled->at(fo.dt) == times_two->at(fo.dt));
  REQUIRE(series_count(TEST_SERIES > 300) == 529);

}


TEST_CASE("WINDOW_SERIES"){

  current_dates active(TEST_SERIES);
  const auto avg = [](const std::vector<double>& v){
    double total = 0;
    for(const auto n : v){
      total += n;
    }
    return total / v.size();
  };

  REQUIRE(verify_equivalency(ma(TEST_SERIES,20), window_series(TEST_SERIES,20,avg)));
  
}


TEST_CASE("SERIES_MAP"){

  current_dates active(TEST_SERIES);
  const auto S = TEST_SERIES;
  
  const auto sum = [](const std::vector<double>& nums){
    double total=0;
    for(const auto n : nums){
      total += n;
    }
    return total;
  };

  REQUIRE(verify_equivalency(series_map(sum, S), S));
  REQUIRE(verify_equivalency(series_map(sum, S, S), S+S));
  REQUIRE(verify_equivalency(series_map(sum, S, S, S), S*3));
  REQUIRE(verify_equivalency(series_map(sum, S, S, S, S), S*4));
  REQUIRE_THROWS(series_map(sum));
}

TEST_CASE("CALIBRATE"){

  current_dates active(TEST_SERIES);
  const auto fd = current_dates::active()->at(0);
  {
    const auto ratio = 100 / TEST_SERIES->at(fd);
    const auto cal = calibrate(TEST_SERIES);
    REQUIRE(verify_equivalency(cal, TEST_SERIES * ratio));
  }
  {
    const auto ratio = 22 / TEST_SERIES->at(fd);
    const auto cal = calibrate(TEST_SERIES,22);
    REQUIRE(verify_equivalency(cal, TEST_SERIES * ratio));
  }
  {
    const auto new_years_eve = parse_jdate("2014-12-31");
    const auto ratio = 22 / TEST_SERIES->at(new_years_eve);
    const auto cal = calibrate(TEST_SERIES,22,new_years_eve);
    REQUIRE(verify_equivalency(cal, TEST_SERIES * ratio));
  }
}

TEST_CASE("ALLOCATION"){

  current_dates active(AAA_SERIES);
  const auto fd = current_dates::active()->at(0);

  {
    std::vector<portion> portions;
    portions.push_back({ AAA_SERIES, 5000});
    portions.push_back({ BBB_SERIES, 5000});
    allocation a { fd, portions };
    std::vector<allocation> allocations { a };
    REQUIRE(verify_equivalency(allocation_equity_line(allocations, 100) * 2,
			       calibrate(AAA_SERIES) + calibrate(BBB_SERIES)));
  }

  {
    std::vector<portion> portions;
    portions.push_back({ AAA_SERIES, 5000});
    portions.push_back({ AAA_SERIES, 1000});
    portions.push_back({ BBB_SERIES, 5000});
    REQUIRE_THROWS(allocation{ fd, portions });
  }
}

TEST_CASE("CAPTURE"){
  current_dates active(TEST_SERIES);

  // regression tests
  REQUIRE(approximates(down_capture(TEST_SERIES, warp(TEST_SERIES,10), 63), 0.5898654044835749));
  REQUIRE(approximates(up_capture(TEST_SERIES, warp(TEST_SERIES,10), 21), 0.4715927637890245));
  
}

TEST_CASE("PREPEND"){
  const auto dt_2000_01_01 = parse_jdate("2000-1-1");
  const auto dt_2012_01_01 = parse_jdate("2012-1-1");
  const auto dt_2013_01_01 = parse_jdate("2013-1-1");
  const auto dt_2013_02_01 = parse_jdate("2013-2-1");
  const auto dt_2013_03_01 = parse_jdate("2013-3-1");
  const auto dt_2013_06_01 = parse_jdate("2013-6-1");
  const auto dt_2014_01_01 = parse_jdate("2014-1-1");
  const auto dt_2015_01_01 = parse_jdate("2015-1-1");
  const auto dt_2016_01_01 = parse_jdate("2016-1-1");

  const auto earlier = ([&](){
      series_builder builder;
      return (builder.insert({dt_2013_01_01, 80})
	      .insert({dt_2013_02_01, 90})
	      .insert({dt_2013_03_01, 100})
	      .construct());
    })();

  const auto later = ([&](){
      series_builder builder;
      return (builder.insert({dt_2013_03_01,1000})
	      .insert({dt_2013_06_01,1130})
	      .construct());
    })();

  const auto too_much_later = ([&](){
      series_builder builder;
      return (builder.insert({dt_2014_01_01, 10000})
	      .insert({dt_2016_01_01, 11111})
	      .construct());
    })();

  {
    auto literal = ([&](){
			   series_builder builder;
			   return (builder.insert({dt_2013_01_01,800})
				   .insert({dt_2013_02_01,900})
				   .insert({dt_2013_03_01, 1000})
				   .insert({dt_2013_06_01, 1130})
				   .construct());
      })();
  
    dateset_builder b(dt_2012_01_01, dt_2015_01_01);
    current_dates active(std::make_shared<dateset>(b.construct()));
    REQUIRE_THROWS(prepend(too_much_later,earlier));
    REQUIRE(verify_equivalency(prepend(later,earlier),literal));
  }

}

/*
  (module* test racket/base
  (require rackunit
  (submod "..")
  "private/test-support.rkt")
  (with-dates (dates #:first '2012-1-1 #:last '2015-1-1)
  (define EARLIER (literal-series '((2013-1-1 80) (2013-2-1 90) (2013-3-1 100))))
  (define LATER (literal-series '((2013-3-1 1000) (2013-6-1 1130))))
  (define TOO-MUCH-LATER (literal-series '((2014-1-1 10000) (2016-1-1 11111))))
  (check-exn
  exn?
  (λ ()
  (with-dates (dates #:first '2000-1-1 #:last (today))
  (prepend TOO-MUCH-LATER #:with-surrogate EARLIER))))
  (check-not-exn
  (λ ()
  (verify-equivalency
  (prepend LATER #:with-surrogate EARLIER)
  (literal-series '((2013-1-1 800) (2013-2-1 900) (2013-3-1 1000) (2013-6-1 1130))))))))

*/


