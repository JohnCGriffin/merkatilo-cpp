
#include "merkatilo.hpp"
#include <algorithm>

namespace merkatilo {

  std::pair<observation,observation> drawdown(series_ptr sp)
  {
    auto obs = series_to_obs(sp);

    if(obs->empty()){
      throw std::runtime_error("no observations");
    }
      

    auto reversed_minima = ([&](){
	auto mn_ob = *(obs->rbegin());
	observations acc;
	acc.push_back(mn_ob);
	for(auto it = obs->rbegin(); it != obs->rend(); ++it){
	  if(mn_ob.val >= it->val){
	    mn_ob = *it;
	    acc.push_back(mn_ob);
	  }
	}
	std::reverse(acc.begin(),acc.end());
	return acc;
      })();

    auto maxima = ([&](){
	auto mx_ob = *(obs->begin());
	observations acc;
	acc.push_back(mx_ob);
	for(auto ob : *obs){
	  if(ob.val > mx_ob.val){
	    mx_ob = ob;
	    acc.push_back(ob);
	  }
	}
	return acc;
      })();

    auto dd_ratio = [](std::pair<observation,observation> dd){
      return dd.second.val / dd.first.val;
    };

    auto dd = std::make_pair(maxima.at(0),maxima.at(0));
    auto minima_it = reversed_minima.begin();

    for(auto mx : maxima){
      while(minima_it != reversed_minima.end() && minima_it->dt <= mx.dt){
	++minima_it;
      }
      if(minima_it == reversed_minima.end()){
	break;
      }
      auto test_dd = std::make_pair(mx,*minima_it);
      if(dd_ratio(test_dd) < dd_ratio(dd)){
	dd = test_dd;
      }
    }

    return dd;
  }
  
}
