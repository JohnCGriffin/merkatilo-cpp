
CXX=g++
CXXFLAGS=-no-pie -g -pg --std=c++14 
CXXFLAGS=-O3 --std=c++14 -fmax-errors=1

OBS = jdate.o dates.o lo.o current_dates.o dump.o series_builder.o \
	ema.o sma.o series.o series_count.o min_max.o \
	first_last_ob.o momentum.o to_signals.o obs_series.o \
	drawdown.o fudge.o volatility.o unrepeated.o repeated.o  \
	warp.o cross.o constant.o equity_line.o reversals.o \
	performance.o conviction.o filter.o overload_series_operators.o \
	window_series.o

test: testing.o testing_main.o $(OBS)
	g++ testing.o testing_main.o $(OBS) -o testing && ./testing

main: merkatilo.hpp.gch main.o $(OBS)
	$(CXX) $(CXXFLAGS) main.o $(OBS) -o main

merkatilo.hpp.gch: merkatilo.hpp
	$(CXX) $(CXXFLAGS) -c merkatilo.hpp

jdate.o: merkatilo.hpp.gch jdate.cpp
	$(CXX) $(CXXFLAGS) -c jdate.cpp

doc:
	doxygen

clean:
	rm -f *.o *.out *.gch main testing && rm -rf html
