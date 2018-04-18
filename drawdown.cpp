
#include "merkatilo.hpp"
#include <algorithm>

namespace merkatilo {

  static drawdown dd_worker (observations_ptr obs){
  
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

    auto dd = drawdown { maxima.at(0),maxima.at(0) };
    auto minima_it = reversed_minima.begin();

    for(const auto& mx : maxima){
      while(minima_it != reversed_minima.end() && minima_it->dt <= mx.dt){
	++minima_it;
      }
      if(minima_it == reversed_minima.end()){
	break;
      }
      auto test_dd = drawdown { mx,*minima_it };
      if(test_dd.residual() < dd.residual()){
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

  drawdown series_drawdown(series_ptr sp, dateset_ptr dates)
  {
    auto obs = series_to_obs(sp, dates);

    if(obs->empty()){
      throw std::out_of_range("no observations");
    }

    return dd_worker(obs);
  }

  static std::vector<drawdown> dds_worker(observations_ptr obs, double max_residual)
  {
    std::vector<drawdown> result;
    if(obs->size() < 2){
      return result;
    }
    auto dd = dd_worker(obs);
    if(dd.max.dt == dd.min.dt){
      return result;
    }
    if(dd.residual() > max_residual){
      return result;
    }
    result.push_back(dd);

    observations left_collector, right_collector;
    for(const auto ob : *obs){
      if(ob.dt <= dd.max.dt){
	left_collector.push_back(ob);
      } else if(ob.dt >= dd.min.dt){
	right_collector.push_back(ob);
      }
    }

    const auto left_result = dds_worker(std::make_shared<observations>(left_collector),
					max_residual);
    const auto right_result = dds_worker(std::make_shared<observations>(right_collector),
					 max_residual);

    for(const auto ob : left_result){
      result.push_back(ob);
    }

    for(const auto ob : right_result){
      result.push_back(ob);
    }

    return result;
  }

  std::vector<drawdown> series_drawdowns(series_ptr sp, double max_residual, dateset_ptr dates)
  {
    const auto obs = series_to_obs(sp, dates);

    if(obs->empty()){
      throw std::out_of_range("no observations");
    }

    auto dds = dds_worker(obs, max_residual);
    std::sort(dds.begin(),
	      dds.end(),
	      [](const drawdown& a, const drawdown& b){
		return a.residual() < b.residual();
	      });
    return dds;
  }

  double drawdown::residual() const
  {
    return this->min.val / this->max.val;
  }

}
