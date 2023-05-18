#ifndef FILTER_DEF
#define FILTER_DEF

#include "pos.cxx"
#include <CGAL/enum.h>
#include <functional>

#include <CGAL/Surface_mesh.h>
#include <CGAL/Cartesian.h>

using TType = double;
using TKernel = CGAL::Cartesian< TType >;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TVertex = TMesh::vertex_index;

// todas las funciones aqui deber recibir unicamente un Pos, y retornar un bool
struct Filter {
	static std::function<bool(Pos)> circle(Pos max) {
		const Pos centro {max.x/2,max.y/2,max.z/2};
		return [centro](Pos p) {
			return centro.x < centro.distancia(p);
		};
	}
	static std::function<bool(Pos)> object(TMesh m, Pos size) {
		Pos bounding_max{0, 0, 0};

		for (TMesh::Vertex_index vi: m.vertices()) {
			int x = m.point(vi).x();
			int y = m.point(vi).y();
			int z = m.point(vi).z();

			if (x > bounding_max.x) {bounding_max.x = x;}
			if (y > bounding_max.y) {bounding_max.x = y;}
			if (z > bounding_max.z) {bounding_max.x = z;}
		}

		Pos step_size{
			bounding_max.x / size.x,
			bounding_max.y / size.y,
			bounding_max.z / size.z
		};
		return nullptr;
		// return [step_size, m](Pos p) {
		// 	TPoint p3{p.x, p.y, p.z};
		// 	m.point(p3);
		// 	CGAL::Oriented_side(m, p3);
		// 	return false;
		// };
	}
};

#endif
