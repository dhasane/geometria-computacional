#ifndef LABYRINTH_SOLVER_DEF
#define LABYRINTH_SOLVER_DEF

#include "labyrinth.cxx"
#include "pos.cxx"

using TKernel = CGAL::Cartesian< double >;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TVertex = TMesh::vertex_index;

struct LabSolver {
	static TMesh solve(TMesh m, Pos start, Pos end) {
		TMesh solution;

		TPoint pstart{start.x, start.y, start.z};
		TPoint pend{end.x, end.y, end.z};

		TMesh::vertex_index vistart;
		TMesh::vertex_index viend;

		for (TMesh::vertex_index vi: m.vertices()) {
			TPoint p = m.point(vi);

			if (p == pstart) {
				vistart = vi;
			}
			if (p == pend) {
				viend = vi;
			}
		}

		std::deque<std::pair<TMesh::Face_index, int>> pending_faces;
		// std::set<TMesh::Face_index> gone_through_faces;

		//       cara actual                  cara anterior      pasos
		std::map<TMesh::face_index, std::pair<TMesh::face_index, int>> gone_through_faces;

		auto first_face = m.face(m.halfedge(vistart));
		pending_faces.push_back({first_face, 0});
		gone_through_faces[first_face] = {first_face, 0};

		bool exit_found = false;
		TMesh::face_index last_face;

		std::cout << "entrada: "<< vistart << "  salida: " << viend << std::endl;

		while(!pending_faces.empty() || !exit_found) {
			TMesh::face_index fi;
			int pasos;
			boost::tie(fi, pasos) = pending_faces.front(); pending_faces.pop_front();

			pasos++;

			for (TMesh::face_index fac: m.faces_around_face(m.halfedge(fi))){
				if (gone_through_faces.find(fac) == gone_through_faces.end() ||
					gone_through_faces[fac].second > pasos)
				{
					gone_through_faces[fac] = {fi, pasos};
					pending_faces.push_back({fac, pasos});
				}

				for (auto vv :m.vertices_around_face(m.halfedge(fac)) ) {
					if (vv == viend) {
						exit_found = true;
						last_face = fac;
					}
				}
			}
		}
		//       cara actual                  cara anterior      pasos
		// std::map<TMesh::face_index, std::pair<TMesh::face_index, int>> gone_through_faces;
		std::pair<TMesh::face_index, int> curr = gone_through_faces[last_face];
		while (curr.first != first_face) {
			std::vector<TMesh::vertex_index> pts;
			for (TMesh::vertex_index tt : m.vertices_around_face(m.halfedge(curr.first))) {
				pts.push_back(
					solution.add_vertex(m.point(tt))
					);
			}
			solution.add_face(pts);
			std::cout << curr.first << "  " << curr.second << std::endl;
			curr = gone_through_faces[curr.first];
		}

		return solution;
	}
};

#endif
