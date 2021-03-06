
CXX=g++
CXXFLAGS=-no-pie -g -pg -fPIC --std=c++14 
CXXFLAGS=-Wpedantic -Wall -fPIC -O3 --std=c++14 

OBS = jdate.o dates.o lo.o current_dates.o dump.o series_builder.o \
	ema.o ma.o series.o series_count.o min_max.o \
	prepend.o first_last_ob.o momentum.o to_signals.o obs_series.o \
	drawdown.o fudge.o volatility.o unrepeated.o repeated.o  \
	warp.o cross.o constant.o equity_line.o reversals.o \
	performance.o conviction.o filter.o overload_series_operators.o \
	window_series.o series_map.o serialize.o calibrate.o \
	allocation_equity_line.o capture.o

libs: libmerkatilo.so libmerkatilo.a

libmerkatilo.so: $(OBS)
	$(CXX) -shared -fPIC -o $@ $(OBS)

libmerkatilo.a: $(OBS)
	ar q $@ $(OBS)

test: merkatilo-test-data merkatilo.hpp.gch testing.o testing_main.o libs
	$(CXX) $(CXXFLAGS) testing.o testing_main.o $(OBS) libmerkatilo.a -o testing && ./testing

merkatilo.hpp.gch: merkatilo.hpp
	$(CXX) $(CXXFLAGS) -c merkatilo.hpp

%.o: %.cpp merkatilo.hpp.gch
	$(CXX) $(CXXFLAGS) -c -o $@ $<

doc:
	doxygen && echo '.contents { max-width : 960px }' >> html/doxygen.css

clean:
	rm -f libmerkatilo.* *.o *.out *.gch benchmark testing && rm -rf html

/tmp/merkatilo-test-data/ema-3.txt:
	@rm -rf /tmp/merkatilo-test-data && (cd /tmp && git clone https://github.com/JohnCGriffin/merkatilo-test-data)

merkatilo-test-data: /tmp/merkatilo-test-data/ema-3.txt
	@test -d /tmp/merkatilo-test-data

bench: libmerkatilo.a standard_bench.o
	@g++ -O3 --std=c++14 standard_bench.o libmerkatilo.a && ./a.out
