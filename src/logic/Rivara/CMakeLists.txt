cmake_minimum_required(VERSION 3.0.0)

find_package(Linq)

add_library(RivaraProductions RivaraAttributes.hpp RivaraAttributes.cpp RivaraP2.hpp RivaraP2.cpp RivaraP1.hpp RivaraP1.cpp RivaraP0.hpp RivaraP0.cpp)
target_link_libraries(RivaraProductions LogicUtils)
target_link_libraries(RivaraProductions Logic)
target_link_libraries(RivaraProductions Models)
target_include_directories(RivaraProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(RivaraProductions PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
message("searching for ${PROJECT_SOURCE_DIR}/src/models/")