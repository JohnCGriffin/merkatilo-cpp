
#include "merkatilo.hpp"

namespace merkatilo {

  series_ptr conviction (series_ptr sp, unsigned N) {

    if(N < 1){
      throw std::runtime_error("N must be >= 1");
    }

    auto dts = current_dates::active();
    const int len = dts->size();
    auto sigs = to_signals(sp);
    auto s = sigs.get();

    double sig_vals[len];

    // remove too-close neighbors
    int last_sig_ndx = -1;
    for(int i=0; i<len; i++){
      auto val = s->at(dts->at(i));
      sig_vals[i] = val;
      if(valid(val)){
	if(last_sig_ndx >= 0 && (i-last_sig_ndx) <= int(N)){
	  sig_vals[i] = sig_vals[last_sig_ndx] = default_value();
	}
	last_sig_ndx = i;
      }
    }

    // warp signals by N
    series_builder builder;
    for(int i=0; i+int(N)<len; i++){
      auto val = sig_vals[i];
      if(valid(val)){
	builder.insert({ dts->at(i+N), val });
      }
    }

    return builder.construct();
  }

}