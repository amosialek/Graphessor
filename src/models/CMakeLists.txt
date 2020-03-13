cmake_minimum_required(VERSION 3.0.0)

add_library(Models Pixel.cpp Pixel.hpp mygraph.hpp GraphessorConstants.hpp CachedGraph.cpp CachedGraph.hpp)
target_include_directories(Models PUBLIC "${PROJECT_SOURCE_DIR}/src/models")