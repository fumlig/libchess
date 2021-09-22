CXXFLAGS ?= -std=c++20 -g -O3  -Wall -Wpedantic
CPPFLAGS ?= -Iinclude

HEADER := include/chess/chess.hpp
DOCS := docs/chess.md
TESTS := $(patsubst test/%.cpp,build/test_%,$(wildcard test/*))

all: test docs

clean:
	rm $(TESTS) $(DOCS)

test: $(TESTS)

docs: $(DOCS)

$(DOCS): $(HEADER)
	mkdir -p $(@D)
	standardese --compilation.standard c++20 --output.prefix docs/ include/chess/chess.hpp
	rm $(@D)/standardese_*
	mv $(@D)/doc_chess.md $@

build/test_%: test/%.cpp $(HEADER)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<
	./$@ 1> /dev/null