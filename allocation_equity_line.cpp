

#include <iostream>
#include <algorithm>
#include <map>
#include <cmath>
#include "merkatilo.hpp"

namespace merkatilo {

  portion::portion (series_ptr sp, double amount) : sp(sp), amount(amount){
    if(amount <= 0.0){
      throw std::invalid_argument("portion amount must be positive");
    }
  }

  allocation::allocation(jdate date, std::vector<portion>& portions) : date(date), portions(portions)
  {
    std::set<series*> pointers;
    for(const auto &p : portions){
      pointers.insert(p.sp.get());
    }
    if(pointers.size() != portions.size()){
      throw std::invalid_argument("duplicate series supplied to allocation");
    }
  }

  struct holding {
    series_ptr sp;
    double shares;
  };

  struct portfolio {
    jdate date;
    std::vector<holding> holdings;
  };

  static auto CASH = constant(1.0);

  static allocation normalize_allocation (const allocation& a)
  {
    double total = 0.0;
    for (const auto &p : a.portions){
      total += p.amount;
    }
    std::vector<portion> new_portions;
    for(const auto &p : a.portions){
      new_portions.push_back({p.sp, p.amount / total});
    }
    if(!new_portions.size()){
      new_portions.push_back(portion{CASH,1});
    }
    // sanity check
    total = 0.0;
    for(const auto &p : new_portions){
      total += p.amount;
    }
    if(std::fabs(total - 1.0) > 0.00001){
      throw std::logic_error("weights of allocation should sum to 1");
    }
    return allocation { a.date, new_portions };
  }

  static double holding_valuations(jdate date, const std::vector<holding>& holdings)
  {
    double total = 0.0;
    for (const auto& h : holdings){
      const auto shares = h.shares;
      const auto sp = h.sp;
      const auto price = sp->at(date);
      if(valid(price)){
	total += (shares * price);
      } else {
	return 0;
      }
    }
    return total;
  }

  static std::vector<portfolio>
  allocations_to_portfolios (const std::vector<allocation>& _allocations)
  {
    std::vector<allocation> allocations;
    std::transform(_allocations.begin(),
		   _allocations.end(),
		   std::back_inserter(allocations),
		   normalize_allocation);

    std::vector<holding> holdings = { holding { CASH, 1 } };

    std::vector<portfolio> result;

    for (const auto &a : allocations){

      const double portfolio_value = holding_valuations(a.date, holdings);

      holdings.clear();
      
      for(const auto& p : a.portions){
	const auto price = p.sp->at(a.date);
	if (!valid(price)){
	  throw std::runtime_error("missing price at " + jdate_to_string(a.date));
	}
	const auto dollars_for_buy = p.amount * portfolio_value;
	const auto shares_to_buy = dollars_for_buy / price;
	holdings.push_back ( {p.sp, shares_to_buy});
      }

      result.push_back ( { a.date, holdings });
    }

    return result;
  }

  /// Given a vector of allocations and an initial value (default 100), develop
  /// an equity line that reflects the continuous value of the allocation system.
  
  series_ptr allocation_equity_line (std::vector<allocation>& allocations, double initial_value)
  {
    const auto portfolio_history = allocations_to_portfolios(allocations);

    std::map<jdate,std::vector<holding>> holdings_by_date;
    for(const auto& p : portfolio_history){
      holdings_by_date.insert ({ p.date, p.holdings });
    }

    const auto fd = holdings_by_date.begin()->first;
    std::vector<holding> holdings { holding { CASH, 1 }};

    series_builder builder;
    
    for (auto dt = fd; dt < today(); dt++){
      const double portfolio_valuation = holding_valuations(dt, holdings);
      const auto find = holdings_by_date.find(dt);
      if(find != holdings_by_date.end()){
	if(!portfolio_valuation){
	  std::runtime_error("missing series observation at allocation date "
			     + jdate_to_string(dt));
	}
	holdings = find->second;
      }	
      if(portfolio_valuation){
      	builder.insert({dt, initial_value * portfolio_valuation});
      }
    }

    return builder.construct();
  }
								  
}
