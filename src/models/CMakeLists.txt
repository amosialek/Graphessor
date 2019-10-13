cmake_minimum_required(VERSION 3.0.0)

add_library(Models Pixel.cpp Pixel.hpp VertexBase.hpp VertexBase.cpp Edge.hpp Edge.cpp)
target_include_directories(Models PUBLIC "${PROJECT_SOURCE_DIR}/src/models")