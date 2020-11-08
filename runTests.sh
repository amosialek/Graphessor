#!/bin/bash
make -j5
cd build
./example #2> ../test_graph.txt
cd ..
./graph.sh test_graph.txt
