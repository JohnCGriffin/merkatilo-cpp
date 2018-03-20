

#include "catch.hpp"

#include "merkatilo.hpp"
#include <sstream>
#include <exception>


using namespace merkatilo;


static bool approximates(double a, double b, double epsilon=0.00001){
  return (std::abs(a-b) < epsilon);
}


static bool verify_equivalency(series_ptr a, series_ptr b)
{
  for(auto dt : *current_dates::active()){
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
    auto a_val = a->at(dt);
    if(!valid(a_val)){
      continue;
    }
    auto b_val = b->at(dt);
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

TEST_CASE("Test that load worked"){
  current_dates active(TEST_SERIES);
  REQUIRE(series_count(TEST_SERIES) == 754);
}

TEST_CASE("EMA-FRACTIONAL"){

  auto EMA_3_SERIES = test_lo("ema-3");
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
  auto mm = min_max_obs(TEST_SERIES);

  SECTION("check max observation"){
    REQUIRE(jdate_to_string(mm.second.dt) == "2014-09-18");
  }

  SECTION("check min observation"){
    REQUIRE(jdate_to_string(mm.first.dt) == "2012-01-13");
  }

}

TEST_CASE("FIRST-LAST"){

  dateset_builder builder(parse_jdate("2010-1-1"),today());
  auto dts = std::make_shared<dateset>(builder.construct());
  current_dates active(dts);

  SECTION("check first observation"){
    REQUIRE(jdate_to_string(first_ob(TEST_SERIES).dt) == "2012-01-03");
  }

  SECTION("check last observation"){
    REQUIRE(jdate_to_string(last_ob(TEST_SERIES).dt) == "2014-12-31");
  }
  
}

TEST_CASE("MOMENTUM"){

  auto MO_3_SERIES = test_lo("mo-3");
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
    auto sigs = to_signals(mo(TEST_SERIES,240));
    auto obs = observations {
      { parse_jdate("2012-12-17"), 1 },
      { parse_jdate("2013-08-30"), -1 },
      { parse_jdate("2013-9-3"), 1},
      { parse_jdate("2014-12-12"), -1 },
      { parse_jdate("2014-12-19"), 1}
    };
    auto literal = obs_to_series(std::make_shared<observations>(obs));
    REQUIRE(verify_equivalency(literal,sigs));
  }

}

TEST_CASE("DRAWDOWN"){

  current_dates active(TEST_SERIES);

  SECTION("drawdown generates known correct values"){
    auto dd = drawdown(TEST_SERIES);
    REQUIRE(jdate_to_string(dd.first.dt) == "2014-09-18");
    REQUIRE(jdate_to_string(dd.second.dt) == "2014-12-16");
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

  auto unrptd = unrepeated(TEST_SERIES);

  REQUIRE(series_count(unrptd) < series_count(TEST_SERIES));
  REQUIRE(verify_equivalency(repeated(unrptd, true),TEST_SERIES));
  REQUIRE(verify_equivalency(TEST_SERIES,repeated(TEST_SERIES)));
  
}


TEST_CASE("WARP"){

  current_dates active(TEST_SERIES);

  auto wa1 = warp(TEST_SERIES,1);
  auto back_again = warp(wa1, -1);

  REQUIRE(series_count(TEST_SERIES) == series_count(back_again)+1);

  current_dates active2(back_again);

  REQUIRE(verify_equivalency(TEST_SERIES,back_again));

}

TEST_CASE("CROSS"){

  current_dates active(TEST_SERIES);
  auto CROSS_EMA_30 = test_lo("cross-ema-30");

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

  auto test_date = parse_jdate("2000-1-1");
  
  REQUIRE(constant(123.456)->at(test_date) == 123.456);
  REQUIRE_THROWS(constant(default_value()));
}

TEST_CASE("EQUITYLINE"){

 current_dates active(TEST_SERIES);
 auto EQUITYLINE_EMA_10 = test_lo("equityline-ema-10");
 auto crossed = cross(ema(TEST_SERIES,10), TEST_SERIES);
 REQUIRE(verify_equivalency(equity_line(TEST_SERIES,crossed),
			    EQUITYLINE_EMA_10));

}


TEST_CASE("REVERSE"){

  current_dates active(TEST_SERIES);
  auto REVERSALS_95_105 = test_lo("reversals-95-105");
  auto REVERSALS_91_109 = test_lo("reversals-91-109");

  REQUIRE(verify_equivalency(reversals(TEST_SERIES,0.95,1.05),
			     REVERSALS_95_105));

  REQUIRE(verify_equivalency(reversals(TEST_SERIES,0.91,1.09),
			     REVERSALS_91_109));
  
}

TEST_CASE("PERFORMANCE"){

  current_dates active(TEST_SERIES);
  auto magical_sigs = nostradamus(TEST_SERIES, .97, 1.03);
  auto magical_performance = investment_performance(TEST_SERIES, magical_sigs);

  REQUIRE(approximates(gpa(TEST_SERIES), 0.07688365986138823));
  
  REQUIRE(approximates(0.48832261262864174, magical_performance.annualized_gain));
  REQUIRE(approximates(0.9712643678160918, magical_performance.drawdown_residual));
  REQUIRE(approximates(0.9278986478200268, magical_performance.volatility_residual));
  REQUIRE(approximates(0.67828418230563, magical_performance.long_ratio));
  REQUIRE(approximates(31,magical_performance.trades));
  
}

TEST_CASE("CONVICTION"){

  current_dates active(TEST_SERIES);
  auto MO_5_CONVICTION_4 = test_lo("mo-5-conviction-4");

  REQUIRE(verify_equivalency(conviction(mo(TEST_SERIES,5),4),
			     MO_5_CONVICTION_4));
			     
}


TEST_CASE("FILTER"){

  current_dates active(TEST_SERIES);
  REQUIRE(2 == series_count(filter(TEST_SERIES, [](double val){ return val > 360;})));
}


TEST_CASE("ARITHMETIC OPERATORS"){

  current_dates active(TEST_SERIES);
  auto doubled = TEST_SERIES + TEST_SERIES;
  auto times_two = TEST_SERIES * 2;
  auto fo = first_ob(TEST_SERIES);

  REQUIRE(TEST_SERIES->at(fo.dt) * 2 == doubled->at(fo.dt));
  REQUIRE(doubled->at(fo.dt) == times_two->at(fo.dt));
  REQUIRE(series_count(TEST_SERIES > 300) == 529);

}


TEST_CASE("WINDOW_SERIES"){

  current_dates active(TEST_SERIES);
  auto avg = [](const std::vector<double>& v){
    double total = 0;
    for(auto n : v){
      total += n;
    }
    return total / v.size();
  };

  REQUIRE(verify_equivalency(ma(TEST_SERIES,20), window_series(TEST_SERIES,20,avg)));
  
}


TEST_CASE("SERIES_MAP"){

  current_dates active(TEST_SERIES);
  auto S = TEST_SERIES;
  
  auto sum = [](const std::vector<double>& nums){
    double total=0;
    for(auto n : nums){
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
  auto fd = current_dates::active()->at(0);
  {
    auto ratio = 100 / TEST_SERIES->at(fd);
    auto cal = calibrate(TEST_SERIES);
    REQUIRE(verify_equivalency(cal, TEST_SERIES * ratio));
  }
  {
    auto ratio = 22 / TEST_SERIES->at(fd);
    auto cal = calibrate(TEST_SERIES,22);
    REQUIRE(verify_equivalency(cal, TEST_SERIES * ratio));
  }
  {
    auto new_years_eve = parse_jdate("2014-12-31");
    auto ratio = 22 / TEST_SERIES->at(new_years_eve);
    auto cal = calibrate(TEST_SERIES,22,new_years_eve);
    REQUIRE(verify_equivalency(cal, TEST_SERIES * ratio));
  }
}


