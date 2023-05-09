#include <CGAL/Distance_2/Line_2_Line_2.h>
#include <functional>
#include <iostream>

#include <CGAL/Cartesian.h>
#include <CGAL/Surface_mesh.h>

using TKernel = CGAL::Cartesian< double >;
using TReal = TKernel::FT;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TPoint_ind = TMesh::vertex_index;
using TFace = TMesh::Face_index;

struct Circle {
	TPoint center;
	double radius;
};

struct Vecinos {
	TPoint v1;
	TPoint v2;
	TPoint v3;
};

class SimplexMesh {

	TMesh tm;

	Vecinos encontrar_vecinos(TMesh::vertex_index vi) {
		// TODO encontrar los puntos vecinos
		tm.edge(tm.point(point));
		tm.edge(point);

		return Vecinos{};
	}


	Circle get_circ(TMesh::vertex_index point) {
		Vecinos v = encontrar_vecinos(point);

		double x = (v.v1.x() + v.v2.x() + v.v3.x())/3;
		double y = (v.v1.y() + v.v2.y() + v.v3.y())/3;
		double z = (v.v1.z() + v.v2.z() + v.v3.z())/3;

		TPoint center{x, y, z};

		return Circle{ center, squared_distance(center, v.v1) };
	}

	Circle get_sphere(TMesh::vertex_index point) {
		Vecinos v = encontrar_vecinos(point);

	}

};

int main () {
	return 0;
}
