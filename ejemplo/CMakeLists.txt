cmake_minimum_required(VERSION 3.25)

project(ejemplo)

find_package(CGAL REQUIRED)

add_executable(ejecutable miPrimerCgal.cxx)

target_link_libraries(ejecutable CGAL::CGAL)
