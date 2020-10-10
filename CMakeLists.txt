cmake_minimum_required(VERSION 3.10.0)
project(Graphessor VERSION 0.1.0)
if(MSVC)
add_compile_options( /W4 /WX)
else()
add_compile_options( -Wall -Wextra -pedantic -g)
endif()
set(CMAKE_CXX_STANDARD 17)
add_subdirectory("${PROJECT_SOURCE_DIR}/src/models")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/logic")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/logic/Rectangular")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/logic/Rivara")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/executable")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/io")
#include_directories("${PROJECT_SOURCE_DIR}/src/models")
include(CTest)
enable_testing()


set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


################################################
# GOOGLE TEST
################################################
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# # Now simply link against gtest or gtest_main as needed. Eg
# add_executable(example example.cpp)
# target_link_libraries(example gtest_main)
# add_test(NAME example_test COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/example)
# ################################################
include(GoogleTest)
add_executable(example example.cpp)
add_subdirectory("${PROJECT_SOURCE_DIR}/test/logic")
add_subdirectory("${PROJECT_SOURCE_DIR}/test/logic/RivaraTests")
target_link_libraries(example gtest)
target_link_libraries(example Models)
target_link_libraries(example RectangularProductions)
target_link_libraries(example RivaraTest)
target_link_libraries(example LogicTest)
# gtest_discover_tests(example)

add_custom_target(docs docs/create_docs.sh)