
#include "merkatilo.hpp"
#include <algorithm>

namespace merkatilo {

  static obpair dd_worker (observations_ptr obs){
  
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
	for(const auto& ob : *obs){
	  if(ob.val > mx_ob.val){
	    mx_ob = ob;
	    acc.push_back(ob);
	  }
	}
	return acc;
      })();

    auto dd_ratio = [](obpair dd){
      return dd.second.val / dd.first.val;
    };

    auto dd = std::make_pair(maxima.at(0),maxima.at(0));
    auto minima_it = reversed_minima.begin();

    for(const auto& mx : maxima){
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

/**
@brief 
*
* Drawdown contains two observations, the earlier being the higher valued observation 
* and the latter being a lesser valued observation such that that combination of 
* observed date-value pairs represents the greatest loss in value.
*/

  obpair drawdown(series_ptr sp, dateset_ptr dates)
  {
    auto obs = series_to_obs(sp, dates);

    if(obs->empty()){
      throw std::out_of_range("no observations");
    }

    return dd_worker(obs);
  }

  static std::vector<obpair> dds_worker(observations_ptr obs, double max_residual)
  {
    std::vector<obpair> result;
    if(obs->size() < 2){
      return result;
    }
    auto dd = dd_worker(obs);
    if(dd.first.dt == dd.second.dt){
      return result;
    }
    if(drawdown_residual(dd) > max_residual){
      return result;
    }
    result.push_back(dd);

    observations left_collector, right_collector;
    for(auto ob : *obs){
      if(ob.dt <= dd.first.dt){
	left_collector.push_back(ob);
      } else if(ob.dt >= dd.second.dt){
	right_collector.push_back(ob);
      }
    }

    auto left_result = dds_worker(std::make_shared<observations>(left_collector), max_residual);
    auto right_result = dds_worker(std::make_shared<observations>(right_collector), max_residual);

    for(auto ob : left_result){
      result.push_back(ob);
    }

    for(auto ob : right_result){
      result.push_back(ob);
    }

    return result;
  }

  std::vector<obpair> drawdowns(series_ptr sp, double max_residual, dateset_ptr dates)
  {
    auto obs = series_to_obs(sp, dates);

    if(obs->empty()){
      throw std::out_of_range("no observations");
    }

    auto dds = dds_worker(obs, max_residual);
    std::sort(dds.begin(),
	      dds.end(),
	      [](const obpair& a, const obpair& b){
		return (a.second.val/a.first.val) < (b.second.val/b.first.val);
	      });
    return dds;
  }

  double drawdown_residual(obpair ob_pair)
  {
    return ob_pair.second.val / ob_pair.first.val;
  }

}
