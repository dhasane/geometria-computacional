#include "LabToMesh.cxx"
#include "pos.cxx"
#include "filter.cxx"
#include "labyrinth_solver.cxx"

int main(int argc, char** argv)
{
    int t_x = argc >= 2 ? std::atoi(argv[1]) : 20;
    int t_y = argc >= 3 ? std::atoi(argv[2]) : 20;
    int t_z = argc >= 4 ? std::atoi(argv[3]) : 20;

	std::string objpath = argc >= 5 ? argv[4] : "";

	auto l = Labyrinth{
		t_x, t_y, t_z
		, Filter::circle({t_x, t_y, t_z})
		// , Filter::object({t_x, t_y, t_z})
	};

	CGAL::IO::write_OBJ( "mesh.obj", LabToMesh::to_obj(l) );

	// TODO: hay un error cuando el laberinto no es cuadrado
	CGAL::IO::write_OBJ(
		"solution.obj",
		LabSolver::solve(LabToMesh::to_obj(l), l.start, l.end)
		);


    return 0;
}
