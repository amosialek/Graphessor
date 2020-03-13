cmake_minimum_required(VERSION 3.0.0)

add_library(IO writers.cpp writers.hpp)
target_include_directories(IO PUBLIC "${PROJECT_SOURCE_DIR}/src/io")
target_include_directories(IO PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
message("searching for ${PROJECT_SOURCE_DIR}/src/io/")