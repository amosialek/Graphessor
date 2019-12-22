cmake_minimum_required(VERSION 3.0.0)

find_package(Linq)

add_library(Logic Image.cpp Image.hpp Productions.hpp Productions.cpp)
target_link_libraries(Logic linq::linq)
target_include_directories(Logic PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(Logic PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
message("searching for ${PROJECT_SOURCE_DIR}/src/models/")