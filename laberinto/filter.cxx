#ifndef FILTER_DEF
#define FILTER_DEF

#include "pos.cxx"
#include <CGAL/enum.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/AABB_face_graph_triangle_primitive.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Cartesian.h>

#include <functional>

using TType = double;
using TKernel = CGAL::Cartesian< TType >;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TVertex = TMesh::vertex_index;

typedef CGAL::AABB_face_graph_triangle_primitive<TMesh> Primitive;
typedef CGAL::AABB_traits<TKernel, Primitive> Traits;
typedef CGAL::AABB_tree<Traits> Tree;
typedef TKernel::Point_3 Point;
using Segment = TKernel::Segment_3;

// todas las funciones aqui deber recibir unicamente un Pos, y retornar un bool
struct Filter {
	static std::function<bool(Pos)> sphere(Pos max) {
		const Pos centro {max.x/2,max.y/2,max.z/2};
		return [centro](Pos p) {
			return centro.x < centro.distancia(p);
		};
	}

	static std::function<bool(Pos)> piramid(Pos max) {
		return [max](Pos p) {
			std::cout << p.z ;
			int sep = p.z == 0? 0 : (p.z) /2 ;
			std::cout << " : " << sep << std::endl;
			return (p.x - sep <= 0 || max.x < p.x + sep ||
					p.y - sep <= 0 || max.y < p.y + sep
				);
		};
	}

	// poodria ser buena idea mover esto a su propio objeto
	static std::function<bool(Pos)> object(std::string path, Pos size) {
		TMesh m;
		CGAL::IO::read_polygon_mesh(path, m);

		// Tree tree = Tree(faces(m).first, faces(m).second, m);

		Pos bounding_max{0, 0, 0};
		Pos bounding_min{0, 0, 0};
		bool first = true;

		for (TMesh::Vertex_index vi: m.vertices()) {
			Point p = m.point(vi);

			int x = p.x();
			int y = p.y();
			int z = p.z();

			if (x > bounding_max.x || first) {bounding_max.x = x;}
			if (x < bounding_min.x || first) {bounding_min.x = x;}
			if (y > bounding_max.y || first) {bounding_max.y = y;}
			if (y < bounding_min.y || first) {bounding_min.y = y;}
			if (z > bounding_max.z || first) {bounding_max.z = z;}
			if (z < bounding_min.z || first) {bounding_min.z = z;}

			first = false;
		}

		std::cout << bounding_max.to_string() << std::endl;
		std::cout << bounding_min.to_string() << std::endl;

		Pos dist{
			bounding_min.x - bounding_max.x,
			bounding_min.y - bounding_max.y,
			bounding_min.z - bounding_max.z
		};

		float dist_x = sqrt(dist.x * dist.x);
		float dist_y = sqrt(dist.y * dist.y);
		float dist_z = sqrt(dist.z * dist.z);

		// un punto que siempre este afuera del bounding box
		Point OUTSIDE_POINT {
			bounding_min.x + dist_x,
			bounding_min.y - 20,
			bounding_min.z + dist_z,
		};

		std::cout << "OUT: " << OUTSIDE_POINT << std::endl;

		Point step_size{
			dist_x / size.x,
			dist_y / size.y,
			dist_z / size.z
		};

		std::cout << "step: " << step_size << std::endl;

		return [m, OUTSIDE_POINT, step_size, bounding_min](Pos p) {
			// std::cout << step_size << std::endl;
			// TODO: tiene que haber una forma de mover este tree al contexto de la lambda
			Tree tree = Tree(faces(m).first, faces(m).second, m);

			Point b{
				bounding_min.x + step_size.x() * p.x,
				bounding_min.y + step_size.y() * p.y,
				bounding_min.z + step_size.z() * p.z,
			};
			Segment segment_query(OUTSIDE_POINT, b);

			// std::cout << segment_query;
			int inter = tree.number_of_intersected_primitives(segment_query);
			bool ret = false;
			if (inter > 0) {
				// std::cout << " has " << inter;
				ret = inter % 2 == 0;
			}
			// std::cout << std::endl;
			return ret;
		};
	}
};

#endif
