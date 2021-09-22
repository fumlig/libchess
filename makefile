CXXFLAGS ?= -std=c++20 -g -O3  -Wall -Wpedantic
CPPFLAGS ?= -Iinclude

TESTS := $(patsubst test/%.cpp,build/test_%,$(wildcard test/*))

all: test

clean:
	rm $(TESTS)

test: $(TESTS)


build/test_%: test/%.cpp include/chess/chess.hpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<
	./$@