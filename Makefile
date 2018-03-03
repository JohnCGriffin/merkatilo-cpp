
CXX=g++
CXXFLAGS=-no-pie -g -pg --std=c++14 
CXXFLAGS=-O2 --std=c++14 -fmax-errors=1

OBS = jdate.o dates.o lo.o current_dates.o dump.o series_builder.o ema.o sma.o series.o

main: merkatilo.hpp.gch main.o $(OBS)
	$(CXX) $(CXXFLAGS) main.o $(OBS) -o main

merkatilo.hpp.gch: merkatilo.hpp
	$(CXX) $(CXXFLAGS) -c merkatilo.hpp

jdate.o: merkatilo.hpp.gch jdate.cpp
	$(CXX) $(CXXFLAGS) -c jdate.cpp

clean:
	rm -f *.o *.out *.gch main
