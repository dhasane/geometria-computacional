cmake_minimum_required(VERSION 3.25)

project(voronoi)


find_package(CGAL REQUIRED)
## # graficar
## find_package(CGAL COMPONENTS Qt5)
## add_definitions(-DCGAL_USE_BASIC_VIEWER)

add_executable(voronoi voronoi.cxx)

# target_link_libraries(voronoi PUBLIC CGAL::CGAL_Qt5)
target_link_libraries(voronoi CGAL::CGAL)


