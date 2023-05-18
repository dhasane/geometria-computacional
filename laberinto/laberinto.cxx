#include "LabToMesh.cxx"
#include "pos.cxx"
#include "filter.cxx"

int main(int argc, char** argv)
{
    int t_x = argc >= 2 ? std::atoi(argv[1]) : 20;
    int t_y = argc >= 2 ? std::atoi(argv[2]) : 20;
    int t_z = argc >= 2 ? std::atoi(argv[3]) : 20;

	auto l = Labyrinth{
		t_x, t_y, t_z
		, Filter::circle({t_x, t_y, t_z})
	};

	CGAL::IO::write_OBJ( "mesh.obj", LabToMesh::to_obj(l) );

    return 0;
}
