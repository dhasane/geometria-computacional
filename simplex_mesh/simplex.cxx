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

double dot_product(TPoint p1, TPoint p2) {
  return p1.x() * p2.x() + p1.y() * p2.y() + p1.z() * p2.z();
}

struct Circ {
	TPoint center;
	double radius;
};

class Adjacents {
	TPoint center;

	TPoint v1;
	TPoint v2;
	TPoint v3;

	Adjacents (TPoint cntr, TPoint v1, TPoint v2, TPoint v3) {
		this->center = cntr;
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
	}

	double magnitude() {
		return sqrt(
			dot_product(v1, v1) +
			dot_product(v2, v2) +
			dot_product(v3, v3)
			);
	}

	double normal_plano_tangente() {
		double v = dot_product(v1, v2) + dot_product(v2, v3) + dot_product(v3, v1);
		return v / magnitude();
	}

	Circ curcumscribed_circle() {
		double x = (v1.x() + v2.x() + v3.x())/3;
		double y = (v1.y() + v2.y() + v3.y())/3;
		double z = (v1.z() + v2.z() + v3.z())/3;

		TPoint triang_center{x, y, z};

		return Circ{ triang_center, squared_distance(triang_center, v1) }; // S
	}

	Circ circumscribed_sphere() {
		// TODO: no entiendo esto
		sin = r / R;
		cos;

	}
};

class SimplexMesh {

	TMesh tm;

	Vecinos encontrar_vecinos(TMesh::vertex_index vi) {
		// TODO encontrar los puntos vecinos
		tm.edge(tm.point(point));
		tm.edge(point);

		return Vecinos{};
	}



};

int main () {
	return 0;
}
