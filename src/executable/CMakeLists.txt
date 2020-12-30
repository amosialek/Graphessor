cmake_minimum_required(VERSION 3.10.0)
if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog CONFIG REQUIRED)
endif()
link_libraries(stdc++fs)
add_executable(Graphessor main.cpp)
target_link_libraries (Graphessor Interpolations)
target_link_libraries (Graphessor Models)
target_link_libraries (Graphessor IO)
target_link_libraries (Graphessor RectangularProductions)
target_link_libraries (Graphessor RivaraProductions)


find_package(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
TARGET_LINK_LIBRARIES( Graphessor ${Boost_LIBRARIES} )

add_executable(f2bitmap fun2bitmap.cpp)
target_link_libraries (f2bitmap Models)
TARGET_LINK_LIBRARIES(f2bitmap ${Boost_LIBRARIES} )