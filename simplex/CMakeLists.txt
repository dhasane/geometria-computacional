## =========================================================================
## @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
## =========================================================================

# -- Base configuration
cmake_minimum_required(VERSION 3.0)
project(MyConvexHullAlgorithms VERSION 1.0.0.1)

# -- Output dirs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# -- Packages
find_package(Eigen3 3.4 REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()
find_package(CGAL REQUIRED COMPONENTS Qt5)

# -- Subdirs
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(CompGeom_MeshToSimplex bash/MeshToSimplex.cxx)
target_link_libraries(
  CompGeom_MeshToSimplex
  Eigen3::Eigen
  CGAL::CGAL
  CGAL::CGAL_Basic_viewer
  )

add_executable(MarcoAurelio bash/MeshToSimlexMarcoAurelioCampoVec.cxx)
target_link_libraries(
  MarcoAurelio
  Eigen3::Eigen
  CGAL::CGAL
  CGAL::CGAL_Basic_viewer
  )

## eof - CMakeLists.txt
