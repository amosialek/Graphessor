cmake_minimum_required(VERSION 3.0.0)

add_library(LogicUtils Productions.hpp Productions.cpp GraphImageWriter.hpp GraphImageWriter.cpp)
target_include_directories(LogicUtils PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(LogicUtils PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
message("searching for ${PROJECT_SOURCE_DIR}/src/models/")