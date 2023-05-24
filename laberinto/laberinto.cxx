#include "LabToMesh.cxx"
#include "labyrinth.cxx"
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

	CGAL::IO::write_OBJ(
		"solution.obj",
		LabSolver::solve(
			LabToMesh::to_obj(l),
			LabToMesh::real_pos(l.start),
			LabToMesh::real_pos(l.end)
			)
		);

    return 0;
}
