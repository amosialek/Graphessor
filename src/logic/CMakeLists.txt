cmake_minimum_required(VERSION 3.0.0)

find_package(Linq)

add_library(LogicUtils Image.cpp Image.hpp Productions.hpp Productions.cpp)

add_library(Logic P1.hpp P2.hpp P3.hpp P4.hpp P5.hpp P6.hpp P1.cpp P2.cpp P3.cpp P4.cpp P5.cpp P6.cpp)
target_link_libraries(LogicUtils linq::linq)
target_link_libraries(Logic LogicUtils)
target_include_directories(LogicUtils PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(LogicUtils PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
target_include_directories(Logic PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
message("searching for ${PROJECT_SOURCE_DIR}/src/models/")