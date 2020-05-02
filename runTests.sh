#!/bin/bash
make
cd build
./example #2> ../test_graph.txt
cd ..
./graph.sh test_graph.txt
