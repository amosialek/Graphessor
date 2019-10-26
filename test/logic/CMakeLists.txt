cmake_minimum_required(VERSION 3.0.0)

add_library(LogicTest Image_test.cpp)
target_include_directories(LogicTest PUBLIC "${PROJECT_SOURCE_DIR}/test/logic")
target_include_directories(LogicTest PUBLIC "${PROJECT_SOURCE_DIR}/googletest-src/googletest/include")
target_include_directories(LogicTest PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
file(COPY "test_files" DESTINATION "${PROJECT_SOURCE_DIR}/${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("test_files copied to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
