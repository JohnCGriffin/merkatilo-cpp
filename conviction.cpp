
#include "merkatilo.hpp"


namespace merkatilo {

  /// The conviction operator reduces whipsaw signals by delaying the signal 
  /// then producing it only if it is not flipped with the specified number 
  /// of periods.  As a small side benefit, any practical trading strategy 
  /// receives a "heads-up" that a signal is imminent.  

  series_ptr conviction (series_ptr sp, unsigned N, dateset_ptr dates) {

    if(N < 1){
      throw std::runtime_error("N must be >= 1");
    }

    const int len = dates->size();
    auto sigs = to_signals(sp);
    auto s = sigs.get();

    double sig_vals[len];

    int last_sig_ndx = -1;
    for(int i=0; i<len; i++){
      auto val = s->at(dates->at(i));
      sig_vals[i] = val;
      if(valid(val)){
	if(last_sig_ndx >= 0 && (i-last_sig_ndx) <= int(N)){
	  sig_vals[i] = sig_vals[last_sig_ndx] = default_value();
	}
	last_sig_ndx = i;
      }
    }

    series_builder builder;
    for(int i=0; i+int(N)<len; i++){
      auto val = sig_vals[i];
      if(valid(val)){
	builder.insert({ dates->at(i+N), val });
      }
    }

    return builder.construct();
  }

}
