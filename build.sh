#!/usr/bin/env bash

mkdir -p build

# test
g++ -std=c++20 test/perft.cpp -o build/perft -Iinclude -g -O3