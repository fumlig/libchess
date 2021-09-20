#!/usr/bin/env bash

mkdir -p build

# test
clang++ -std=c++20 test/perft.cpp -o build/perft -Iinclude -Wall -Wpedantic -g -O3
clang++ -std=c++20 test/usage.cpp -o build/usage -Iinclude -Wall -Wpedantic -g -O3