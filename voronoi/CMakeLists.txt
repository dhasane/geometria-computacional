cmake_minimum_required(VERSION 3.25)

project(voronoi)

find_package(CGAL REQUIRED)

add_executable(voronoi voronoi.cxx)

target_link_libraries(ejecutable CGAL::CGAL)
