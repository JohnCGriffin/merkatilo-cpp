

#include "catch.hpp"

#include "merkatilo.hpp"
#include <sstream>
#include <exception>


using namespace merkatilo;

bool verify_equivalency(series_ptr a, series_ptr b)
{
  for(auto dt : *current_dates::active()){
    if(valid(a->at(dt)) != valid(b->at(dt))){
      if(valid(a->at(dt))){
	throw std::logic_error(("b missing data at ") + jdate_to_string(dt));
      } else {
	throw std::logic_error(("a missing data at ") + jdate_to_string(dt));
      }
    }
    auto a_val = valueof(a->at(dt));
    auto b_val = valueof(b->at(dt));
    if(std::abs(a_val - b_val) > 0.00001){
      throw std::logic_error(("mismatched series at ") + jdate_to_string(dt));
    }
  }
  return true;
}

series_ptr test_lo (std::string name){
  return lo("/tmp/merkatilo-test-data/" + name + ".txt");
}

auto TEST_SERIES = test_lo("test-series");

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


