#include <CGAL/Distance_2/Line_2_Line_2.h>
#include <CGAL/Surface_mesh/Surface_mesh.h>
#include <functional>
#include <iostream>

#include <CGAL/Cartesian.h>
#include <CGAL/Surface_mesh.h>

using TKernel = CGAL::Cartesian< double >;
using TReal = TKernel::FT;
using TPoint = TKernel::Point_3;
using TLine = TKernel::Line_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TPoint_ind = TMesh::vertex_index;
using TFace = TMesh::Face_index;

// <radio circ, centro circ, radio esfera, centro esfera> simplexParams(mesh, vertexHandle) {
//
// }

double dot_product(TPoint p1, TPoint p2) {
  return
	  p1.x() * p2.x() +
	  p1.y() * p2.y() +
	  p1.z() * p2.z();
}

TPoint cross_product(TPoint p1, TPoint p2) {
	return {
		p1.y() * p2.z() - p1.z() * p2.y(),
		p1.z() * p2.x() - p1.x() * p2.z(),
		p1.x() * p2.y() - p1.y() * p2.x()
	};
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

public:

	// Adjacents (TMesh m, TMesh::vertex_index vi) {
	// 	center = m.point(vi);
	// 	auto he = m.halfedge(vi);

	// 	this->v1 = he;
	// 	this->v2 = he.prev.opp;
	// 	this->v3 = he.prev.opp.prev.opp;
	// }

	Adjacents (TPoint p1, TPoint p2, TPoint p3) {
		this->v1 = p1;
		this->v2 = p2;
		this->v3 = p3;
	}

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

	Circ curcumscribed_circle() {
		// 1. Find the equation of the plane that contains the three
		// points.
		TLine AB {v1, v2};
		TLine AC {v1, v3};
		
		// 2. Find the midpoint of any two sides of the triangle.
		TPoint mid1{(v1.x() + v2.x())/2, (v1.y() + v2.y())/2, (v1.z() + v2.z())/2};
		TPoint mid2{(v1.x() + v3.x())/2, (v1.y() + v3.y())/2, (v1.z() + v3.z())/2};

		// 3. Find the normal vector to the plane containing the
		// triangle.
		auto normal = cross_product(AB.point(), AC.point());

		// 4. Find the intersection of the line passing through the
		// two midpoints with the plane containing the triangle.
		TPoint u = cross_product({mid1.x() - mid2.x(), mid1.y() - mid2.y(), mid1.z() - mid2.z()}, normal);
		TPoint v = cross_product(u, normal);

		// 5. Find the distance between any of the three vertices and
		// the intersection point found in step 4.

		// 6. Find the center of the circumcircle by taking the
		// midpoint of any two sides of the triangle and adding the
		// radius times the normal vector to the plane containing the
		// triangle.
	}

	Circ circumscribed_sphere() {
		// TODO: no entiendo esto
		// Find the equation of the plane that contains the three points

	}
};

class SimplexMesh {

	TMesh tm;

};

int main () {
	TPoint p1 = {1, 0, 0};
    TPoint p2 = {0, 1, 0};
    TPoint p3 = {0, 0, 1};
	return 0;
}
