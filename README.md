
# Merkatilo Time-Series

## Background

The merkatilo time-series library implementations address my personal financial computing research requirements. This is the C++ implementation.  As a C++ library, it's not useful interactively; for that use the Python or Racket implementation.  However, the routines parallel the interpreted implmentations, so converting from a script to C++ code is not difficult.

The purpose of this library is financial computing research and education, not trading with material funds at risk. Use it accordingly.

The code resides at Github under MIT licensing. Corrections, improvements, or other comments are welcome. Please include merkatilo in the subject line.

John Griffin, griffinish at gmail

## Overview

The basic data structures are a time series structure that wraps a simple date-to-number function, a date, and a dateset which is an ordered collection of dates. A time series is represented by struct series which takes only a date->optional-number function and a name. It is the argument to many functions that beget further series instances.

Series creations come in two styles, those requiring a sequence of dates implemented as dateset, and those that do not. For example, the sma procedure creates a new series representing a running average of the input series over a dateset. However, add sums two input series on a date without reference to a date sequence.

Speaking of dates, with merkatilo, they are called jdate, meaning julian date. The julian date coincides with Postgres’ idea of a julian.

Here’s an example that loads the SPY ETF adjusted closing price, does a cross of that series with its 200-period moving average, generating buy (+1) and sell (-1) signals. Finally, it dumps them out in date order, like a dumped spreadsheet.

```c++
#include "merkatilo.hpp"

int main()
{
  auto SPY = lo("SPY");
  current_dates active(SPY);
  auto smoothed = sma(SPY,200);
  auto sigs = cross(slower, SPY);
  dump(SPY, smoother, sigs);
}
```

Please note that if you attempt to do the example above, it will not work. That is because this library manipulates times series; it does not provide financial data. You have to come up with that yourself. If you have no data source, investigate using the St. Louis Federal Reserve FRED database, OECD, and Quandl.








