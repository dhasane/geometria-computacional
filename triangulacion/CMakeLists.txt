cmake_minimum_required(VERSION 3.25)

project(triangulacion)

find_package(CGAL REQUIRED)

add_executable(triangulacion triangulacion.cxx)

target_link_libraries(triangulacion CGAL::CGAL)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
