
## =========================================================================
## @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
## =========================================================================

include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(CompGeom_MeshSphere MeshSphere.cxx)
target_link_libraries(CompGeom_MeshSphere CGAL::CGAL)

## eof - CMakeLists.txt
