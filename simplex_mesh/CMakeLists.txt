cmake_minimum_required(VERSION 3.25)

project(simplex)
find_package(CGAL REQUIRED)
add_executable(simplex simplex.cxx)
target_link_libraries(simplex CGAL::CGAL)


