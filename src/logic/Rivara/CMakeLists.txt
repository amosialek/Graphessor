cmake_minimum_required(VERSION 3.0.0)

find_package(Linq)

add_library(RivaraProductions RivaraAttributes.hpp RivaraAttributes.cpp P1.hpp P1.cpp P0.hpp P0.cpp)
target_link_libraries(RivaraProductions LogicUtils)
target_link_libraries(RivaraProductions Logic)
target_link_libraries(RivaraProductions Models)
target_include_directories(RivaraProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(RivaraProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
message("searching for ${PROJECT_SOURCE_DIR}/src/models/")