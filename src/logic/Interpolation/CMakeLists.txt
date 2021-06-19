cmake_minimum_required(VERSION 3.0.0)

add_library(Interpolations RectangularInterpolation.hpp RectangularInterpolation.cpp InterpolationHelper.hpp InterpolationHelper.cpp)
target_link_libraries(Interpolations Models)
target_include_directories(Interpolations PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_include_directories(Interpolations PUBLIC "${PROJECT_SOURCE_DIR}/src/logic")
target_include_directories(Interpolations PUBLIC "${PROJECT_SOURCE_DIR}/src/logic/Interpolation")
