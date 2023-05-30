#include <iostream>
#include <map>
#include <deque>
#include <queue>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

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

const double PI = acos(-1);
const double PI_h = acos(-1)/2;

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

std::string epoint_str(epoint e) {
	std::ostringstream s;
	s << e[0] << ", " << e[1] << ", " << e[2];
	return s.str();
}

double L(TReal r, float d, float phi) {
	std::cout << " pi es " << PI << std::endl;

	double e = phi < PI_h ? 1 : -1;

	double r_q = (r * r);
	double d_q = (d * d);
	double tanphi_q = std::tan(phi);
	double up = r_q - d_q * tanphi_q ;
	double down = e * sqrt(r_q + (r_q - d_q)*(tanphi_q*tanphi_q) + r);

	return up / down;
};

float distancia(epoint &e1, epoint &e2) {
	epoint dist{e1[0] - e2[0], e1[1] - e2[1], e1[2] - e2[2]};
	return sqrt(dist[0] * dist[0] +
				dist[1] * dist[1] +
				dist[2] * dist[2]);
}

epoint new_pos(
	Eigen::Matrix<TReal, 3, 3> Rxy, epoint P,
	epoint Pn1, epoint Pn2, epoint Pn3,
	epoint normal,
	float phi = 40.0,
	float alpha = 0.5,
	float beta = 0.5,
	float y_chistosa = 0.5
	) {

	// Eigen::Matrix<TReal, 3, 1>
	// 	v2_2d = Rxy * (Pn2 - Pn1),
	// 	v3_2d = Rxy * (Pn3 - Pn1);

	// // Circumcircle
	// Circ circle = circumcircle(Pn1, v2_2d, v3_2d, Rxy);

	// // Circumsphere
	// Circ sphere = circumsphere(P, Pn1, Pn2, Pn3);

	// epoint
	// 	F   = Rxy * P;

	// epoint f_tang = circle.first - F;

	// float r = circle.second;
	// float d = distancia(F, circle.first);
	// epoint f_norm = (L(r, d, phi)) * normal;

	// epoint F_int = f_tang + f_norm;


	// epoint F_ext = {};

	// epoint n_Pi = (P + (1 - y_chistosa) * (P) + alpha * F_int + beta * F_ext);


	// return n_Pi;

	// Esto logra lo mismo que se tenia antes
	return circle.first;
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
		Eigen::Matrix<TReal, 3, 1> v, v1, v2, v3;
		v  << p[0] , p[1] , p[2];
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

		epoint new_F1 = new_pos(Rxy, v, v1, v2, v3, {n[0], n[1], n[2]});
		changes.push_back({new_F1[0], new_F1[1], new_F1[2]});
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

void prettify_simplex(TSimplex &simplex, TReal phi, int max_loops = 20, bool print = false) {
	for (int a = 0 ; a < max_loops; a++) {
		prettify_simplex_step(simplex, phi);
		if (print)
			CGAL::draw( simplex );

	}
}

// -------------------------------------------------------------------------
int main( int argc, char** argv )
{
  TMesh mesh;
  TSimplex simplex;

  int reps = argc >= 3 ? std::atoi(argv[2]) : 5;

  CGAL::IO::read_STL( argv[ 1 ], mesh );
  build_simplex_mesh( simplex, mesh );

  prettify_simplex(simplex, 5, reps, true);

  CGAL::draw( simplex );

  return( EXIT_SUCCESS );
}

// eof - $RCSfile$
