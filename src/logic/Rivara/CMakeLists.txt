cmake_minimum_required(VERSION 3.0.0)

find_package(Linq)

add_library(RivaraProductions P1.hpp P1.cpp)
target_link_libraries(RivaraProductions LogicUtils)
target_include_directories(LogicUtils PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(LogicUtils PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
target_include_directories(Logic PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
message("searching for ${PROJECT_SOURCE_DIR}/src/models/")