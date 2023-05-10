cmake_minimum_required(VERSION 3.25)

project(laberinto)

find_package(CGAL REQUIRED)

add_executable(laberinto laberinto.cxx)

target_link_libraries(laberinto CGAL::CGAL)
