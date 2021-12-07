CXXFLAGS ?= -std=c++2a -g -O3  -Wall -Wpedantic
CPPFLAGS ?= -I.

SOURCES = $(wildcard chess/*.cpp)
HEADERS = $(wildcard chess/*.hpp)
TESTS := $(patsubst tests/%.cpp,build/test_%,$(wildcard tests/*))

all: tests

clean:
	rm $(TESTS)

tests: $(TESTS)

build/test_%: tests/%.cpp $(HEADERS) $(SOURCES)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< $(SOURCES)
	./$@ 1> /dev/null
