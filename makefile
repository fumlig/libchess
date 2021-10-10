CXXFLAGS ?= -std=c++20 -g -O3  -Wall -Wpedantic
CPPFLAGS ?= -I.

HEADER := chess/chess.hpp
TESTS := $(patsubst tests/%.cpp,build/test_%,$(wildcard tests/*))

all: tests

clean:
	rm $(TESTS)

tests: $(TESTS)

build/test_%: tests/%.cpp $(HEADER)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<
	./$@ 1> /dev/null