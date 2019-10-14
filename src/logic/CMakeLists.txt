cmake_minimum_required(VERSION 3.0.0)

add_library(Logic Image.cpp Image.hpp)
target_include_directories(Logic PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")