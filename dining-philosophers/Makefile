CXX=g++
CXXFLAGS=-g -O0 -std=c++11
LIBS = -lpthread -lm

all: primes dining-philosophers dining-philosophers-extra

primes: primes.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

dining-philosophers: dining-philosophers.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

dining-philosophers-extra: dining-philosophers-extra.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

vi: dining-philosophers-extra.cpp
	vim $<

test: dining-philosophers-extra
	./$< 5

test-gdb: dining-philosophers
	gdb --args ./$< 5


.PHONY: clean
clean:
	rm -f primes dining-philosophers \
	      dining-philosophers-extra
