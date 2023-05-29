#include <iostream>
#include <map>
#include <deque>
#include <queue>
#include <set>
#include <vector>
#include <string>
#include <sstream>

#include <Eigen/Core>
#include <Eigen/Dense>

#include <CGAL/Cartesian.h>
#include <CGAL/Kernel_traits.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/draw_polyhedron.h>
#include <CGAL/draw_surface_mesh.h>

#include "simplex_dark_magic.cxx"

// -------------------------------------------------------------------------
// Types
using TKernel	= CGAL::Cartesian< float >;
using TReal		= TKernel::FT;
using TPoint	= TKernel::Point_3;
using TMesh		= CGAL::Surface_mesh< TPoint >;
using TSimplex	= CGAL::Polyhedron_3< TKernel >;
using vec3		= TKernel::Vector_3;



using epoint = Eigen::Matrix<TReal, 3, 1>;

using Circ = std::pair<epoint, TReal>;

struct PT {
	epoint center;
	PT(epoint center) {
		this->center = center;
	}
	PT(vec3 p) {
		this->center << p[0], p[1], p[2];
	}
	TReal x() const {
		return center[0];
	}
	TReal y() const {
		return center[1];
	}
	TReal z() const {
		return center[2];
	}

	TPoint to_point() {
		return {x(), y(), z()};
	}

	std::string to_string() const {
		std::ostringstream s;
		s << "("
		  << x() << ", "
		  << y() << ", "
		  << z() << ")";
		return s.str();
	}
};

void get_normal(vec3 &n, vec3 p1, vec3 p2, vec3 p3) {
	n += CGAL::cross_product( p2, p3 );
	n += CGAL::cross_product( p3, p1 );
	n /= std::sqrt( n.squared_length( ) );
}

// epoint new_pos(
// 	Circ circle, Circ sphere,
// 	Eigen::Matrix<TReal, 3, 3> Rxy, epoint P,
// 	epoint P1, epoint P2, epoint P3
// 	) {
// 	epoint
// 		F   = Rxy * P;
//
// 	float
// 		e1 = 3,
// 		e2 = 3,
// 		e3 = 3;
//
// 	epoint F_centroid = circle.first;
//
// 	epoint f_tang = F_centroid - F;
//
// 	// std::cout << std::endl << ">> "
// 	// 		  << PT{F}.to_string() << "(" << PT{pv}.to_string() << ")"<< " -> " << PT{F_centroid}.to_string() << " : "
// 	// 		  << PT{f_tang}.to_string()
// 	// 		  << std::endl;
//
// 	// return Rxy.transpose() * (F_centroid + f_tang);
// 	return Rxy.transpose() * F_centroid;
// }

epoint new_pos(
	Circ circle, Circ sphere,
	Eigen::Matrix<TReal, 3, 3> Rxy, epoint P,
	epoint P1, epoint P2, epoint P3
	) {

	Eigen::Matrix<TReal, 3, 1>
		pv  = Rxy * P,
		pv1 = Rxy * P1,
		pv2 = Rxy * P2,
		pv3 = Rxy * P3;

	// TODO: esto se consigue mal
	Eigen::Matrix<TReal, 3, 1> centroid{
		(pv1.x() + pv2.x() + pv3.x())/3 ,
		(pv1.y() + pv2.y() + pv3.y())/3 ,
		(pv1.z() + pv2.z() + pv3.z())/3 ,
	};
	// std::cout << circ.to_string() << " " << pos.to_string() << " -> " << PT{centroid}.to_string() << std::endl;
	Eigen::Matrix<TReal, 3, 1> ret = Rxy.transpose() * centroid;

	return ret;
}

bool prettify_simplex_step(TSimplex &simplex, TReal phi) {
	std::deque<TPoint> changes;
	for(
		TSimplex::Vertex_iterator vIt = simplex.vertices_begin( );
		vIt != simplex.vertices_end( );
		++vIt
		)
	{
		TSimplex::Halfedge_handle h1 = vIt->halfedge( );
		TSimplex::Halfedge_handle h2 = h1->opposite()->prev();
		TSimplex::Halfedge_handle h3 = h2->opposite()->prev();

		// Neighboring points
		vec3 p  = h1->vertex()->point() - CGAL::ORIGIN;
		vec3 p1 = h1->opposite()->vertex()->point() - CGAL::ORIGIN;
		vec3 p2 = h2->opposite()->vertex()->point() - CGAL::ORIGIN;
		vec3 p3 = h3->opposite()->vertex()->point() - CGAL::ORIGIN;

		// Normal
		vec3 n = CGAL::cross_product(p1, p2);
		get_normal(n, p1, p2, p3);

		// Transformation to XY plane
		Eigen::Matrix<TReal, 3, 3> Rxy;
		Eigen::Matrix<TReal, 3, 1> v, v1, v2, v3, v2_2d, v3_2d;
		v << p[0], p[1], p[2];
		v1 << p1[0], p1[1], p1[2];
		v2 << p2[0], p2[1], p2[2];
		v3 << p3[0], p3[1], p3[2];

		TReal ct = n[2];
		TReal st = std::sqrt((n[0] * n[0]) + (n[1] * n[1]));
		TReal u1 = n[1] / st;
		TReal u2 = -n[0] / st;

		Rxy(0, 0) = ct + (u1 * u1 * (1.0 - ct));
		Rxy(1, 1) = ct + (u2 * u2 * (1.0 - ct));
		Rxy(2, 2) = ct;
		Rxy(0, 1) = u1 * u2 * (1.0 - ct);
		Rxy(1, 0) = u1 * u2 * (1.0 - ct);
		Rxy(0, 2) = u2 * st;
		Rxy(2, 0) = -u2 * st;
		Rxy(1, 2) = -u1 * st;
		Rxy(2, 1) = u1 * st;

		v2_2d = Rxy * (v2 - v1);
		v3_2d = Rxy * (v3 - v1);

		// Circumcircle
		Circ circ = circumcircle(v1, v2_2d, v3_2d, Rxy);

		// Circumsphere
		Circ sph = circumsphere(v, v1, v2, v3);

		epoint new_F1 = new_pos(circ, sph, Rxy, v, v1, v2, v3);

		TPoint nF1{new_F1[0], new_F1[1], new_F1[2]};

		std::cout << "[" <<
			circ.first << " " <<
			sph.first  << "]"
			<< h1->vertex()->point() << " -> " << nF1
				  << std::endl;
		changes.push_back(nF1);
	}

	bool point_movement = false;
	for(
		TSimplex::Vertex_iterator vIt = simplex.vertices_begin( );
		vIt != simplex.vertices_end( );
		++vIt
		)
	{
		point_movement &= vIt->point() != changes.front();
		// TPoint pt = vIt->point();
		vIt->point() = changes.front();
		changes.pop_front();
	}
	return point_movement;
}

void prettify_simplex(TSimplex &simplex, TReal phi, int max_loops = 20) {
	for (int a = 0 ; a < max_loops; a++) {
		prettify_simplex_step(simplex, phi);
	}
}

// -------------------------------------------------------------------------
int main( int argc, char** argv )
{
  TMesh mesh;
  TSimplex simplex;

  CGAL::IO::read_STL( argv[ 1 ], mesh );
  build_simplex_mesh( simplex, mesh );

  prettify_simplex(simplex, 5, 5);

  CGAL::draw( simplex );

  return( EXIT_SUCCESS );
}

// eof - $RCSfile$
