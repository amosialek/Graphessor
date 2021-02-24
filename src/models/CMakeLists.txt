cmake_minimum_required(VERSION 3.0.0)

find_package(LAPACK)
find_library(LAPACKE_LIBRARY
          NAMES "liblapacke.a"
          HINTS "/usr/lib/")
if(LAPACK_FOUND)
   set(lapackblas_libraries ${LAPACK_LIBRARIES})
else ()
   message(FATAL_ERROR, "no lapack found")
endif()

add_library(Models Pixel.cpp Pixel.hpp mygraph.hpp GraphessorConstants.hpp CachedGraph.cpp CachedGraph.hpp Image.hpp Image.cpp ImageMagnifier.hpp ImageMagnifier.cpp Array2D.hpp Array2D.cpp PbiHelper.hpp PbiHelper.cpp)
target_include_directories(Models PUBLIC "${PROJECT_SOURCE_DIR}/src/models")
target_link_libraries(Models ${LAPACKE_LIBRARY})
target_link_libraries(Models ${lapackblas_libraries})
