cmake_minimum_required(VERSION 3.25)

project(intersecciones)

find_package(CGAL REQUIRED)

add_executable(ejecutable intersecciones.cxx)

target_link_libraries(ejecutable CGAL::CGAL)
