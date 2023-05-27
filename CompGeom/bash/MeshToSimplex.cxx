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

struct PT {
	// di que algo te confunde sin decir que te confunde
	Eigen::Matrix<TReal, 3, 1> center;
	PT(Eigen::Matrix<TReal, 3, 1> center) {
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

struct Circ {
	PT center;
	TReal radius;

	std::string to_string() const {
		std::ostringstream s;
		s << "("
		  << center.x() << ", "
		  << center.y() << ", "
		  << center.z() << ": "
		  << radius << ")";
		return s.str();
	}
};

void get_normal(vec3 &n, vec3 p1, vec3 p2, vec3 p3) {
	n += CGAL::cross_product( p2, p3 );
	n += CGAL::cross_product( p3, p1 );
	n /= std::sqrt( n.squared_length( ) );
}

Circ circumcircle(
	Eigen::Matrix< TReal, 3, 1 > v1,
	Eigen::Matrix< TReal, 3, 1 > v2_2d,
	Eigen::Matrix< TReal, 3, 1 > v3_2d,
	Eigen::Matrix< TReal, 3, 3 > Rxy
	) {
	// Circumcircle
	Eigen::Matrix< TReal, 3, 3 > a, bx, by;
	a <<
		0, 0, 1,
		v2_2d( 0 ), v2_2d( 1 ), 1,
		v3_2d( 0 ), v3_2d( 1 ), 1;
	bx <<
		0, 0, 1,
		v2_2d.transpose( ) * v2_2d, v2_2d( 1 ), 1,
		v3_2d.transpose( ) * v3_2d, v3_2d( 1 ), 1;
	by <<
		0, 0, 1,
		v2_2d.transpose( ) * v2_2d, v2_2d( 0 ), 1,
		v3_2d.transpose( ) * v3_2d, v3_2d( 0 ), 1;

    Eigen::Matrix< TReal, 3, 1 > cc;
    cc <<
		bx.determinant( ) / ( 2 * a.determinant( ) ),
		by.determinant( ) / ( -2 * a.determinant( ) ),
		0;
    TReal rc = ( cc.transpose( ) * cc ).array( ).sqrt( )( 0, 0 );
    cc = ( Rxy.transpose( ) * cc ) + v1;

    // std::cout << cc.transpose( ) << " : ";
    // std::cout << rc << std::endl;
	return Circ{{cc.transpose()}, rc};
}

Circ circumsphere(
	Eigen::Matrix< TReal, 3, 1 > v,
	Eigen::Matrix< TReal, 3, 1 > v1,
	Eigen::Matrix< TReal, 3, 1 > v2,
	Eigen::Matrix< TReal, 3, 1 > v3
	) {
	Eigen::Matrix< TReal, 4, 4 > A, Dx, Dy, Dz;
	A <<
		v.transpose( ), 1,
		v1.transpose( ), 1,
		v2.transpose( ), 1,
		v3.transpose( ), 1;
	Dx <<
		v.transpose( ) * v, v( 1 ), v( 2 ), 1,
		v1.transpose( ) * v1, v1( 1 ), v1( 2 ), 1,
		v2.transpose( ) * v2, v2( 1 ), v2( 2 ), 1,
		v3.transpose( ) * v3, v3( 1 ), v3( 2 ), 1;
	Dy <<
		v.transpose( ) * v, v( 0 ), v( 2 ), 1,
		v1.transpose( ) * v1, v1( 0 ), v1( 2 ), 1,
		v2.transpose( ) * v2, v2( 0 ), v2( 2 ), 1,
		v3.transpose( ) * v3, v3( 0 ), v3( 2 ), 1;
	Dz <<
		v.transpose( ) * v, v( 0 ), v( 1 ), 1,
		v1.transpose( ) * v1, v1( 0 ), v1( 1 ), 1,
		v2.transpose( ) * v2, v2( 0 ), v2( 1 ), 1,
		v3.transpose( ) * v3, v3( 0 ), v3( 1 ), 1;

	Eigen::Matrix< TReal, 3, 1 > cs;
	cs <<
		Dx.determinant( ) / ( 2 * A.determinant( ) ),
		Dy.determinant( ) / ( -2 * A.determinant( ) ),
		Dz.determinant( ) / ( 2 * A.determinant( ) );
	TReal rs = ( ( cs - v ).transpose( ) * ( cs - v ) ).array( ).sqrt( )( 0, 0 );

	// std::cout << cs.transpose( ) << " : " << rs << std::endl;
	return Circ{ {cs.transpose()}, rs };
}

TPoint new_pos(Eigen::Matrix<TReal, 3, 3> Rxy,
		   Eigen::Matrix<TReal, 3, 1> v,
		   Eigen::Matrix<TReal, 3, 1> v1,
		   Eigen::Matrix<TReal, 3, 1> v2,
		   Eigen::Matrix<TReal, 3, 1> v3
	) {

	Eigen::Matrix<TReal, 3, 1>
		pv = Rxy * v,
		pv1 = Rxy * v1,
		pv2 = Rxy * v2,
		pv3 = Rxy * v3;

	// TODO: esto se consigue mal
	Eigen::Matrix<TReal, 3, 1> centroid{
		(pv1.x() + pv2.x() + pv3.x())/3 ,
		(pv1.y() + pv2.y() + pv3.y())/3 ,
		(pv1.z() + pv2.z() + pv3.z())/3 ,
	};
	// std::cout << circ.to_string() << " " << pos.to_string() << " -> " << PT{centroid}.to_string() << std::endl;
	Eigen::Matrix<TReal, 3, 1> ret = Rxy.transpose() * centroid;

	return {ret[0], ret[1], ret[2]};
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

		PT ptF1{Rxy * v};

		TPoint new_F1 = new_pos(Rxy, v, v1, v2, v3);

		// std::cout << "[" <<
		// 	circ.center.to_string() << " " <<
		// 	sph.center.to_string()  << "]"
		// 	// << ptF1.to_string() << " -> " << new_F1.to_string()
		// 		  << std::endl;
		changes.push_back(new_F1);
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

void prettify_simplex(TSimplex &simplex, TReal phi) {
	int max_loops = 20;

	// TODO: encontrar por que se alarga
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

  prettify_simplex(simplex, 5);

  CGAL::draw( simplex );

  return( EXIT_SUCCESS );
}

// eof - $RCSfile$
