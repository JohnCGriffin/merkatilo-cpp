

#include "catch.hpp"

#include "merkatilo.hpp"
#include <sstream>
#include <exception>


using namespace merkatilo;

static bool approximates(double a, double b, double epsilon=0.00001){
  return std::abs(a-b) < epsilon;
}


static bool verify_equivalency(series_ptr a, series_ptr b)
{
  for(auto dt : *current_dates::active()){
    if(valid(a->at(dt)) != valid(b->at(dt))){
      if(valid(a->at(dt))){
	throw std::logic_error(("second series  missing data at ") + jdate_to_string(dt));
      } else {
	throw std::logic_error(("first series missing data at ") + jdate_to_string(dt));
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
	  << a_val << " vs. " << b_val;
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

TEST_CASE("EMA"){

  auto EMA_3_SERIES = test_lo("ema-3");
  current_dates active(TEST_SERIES);

  SECTION("ema generates known correct values"){
    REQUIRE(verify_equivalency(ema(TEST_SERIES,3), EMA_3_SERIES));
  }

  SECTION("ema length is equivalent to input"){
    REQUIRE(series_count(TEST_SERIES) == series_count(ema(TEST_SERIES,20)));
  }

  SECTION("handling bad argument"){
    REQUIRE_THROWS(ema(TEST_SERIES,1));
  }
}

TEST_CASE("SMA"){

  auto SMA_3_SERIES = test_lo("sma-3");
  current_dates active(TEST_SERIES);

  SECTION("sma generates known correct values"){
    REQUIRE(verify_equivalency(sma(TEST_SERIES,3), SMA_3_SERIES));
  }

  SECTION("sma generate len(input)-N+1 output values"){
    unsigned N = 10;
    REQUIRE(series_count(sma(TEST_SERIES,N)) == series_count(TEST_SERIES)-N+1);
  }

  SECTION("handling bad period argument"){
    REQUIRE_THROWS(sma(TEST_SERIES,1));
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

  SECTION("sma generate len(input)-N output values"){
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



