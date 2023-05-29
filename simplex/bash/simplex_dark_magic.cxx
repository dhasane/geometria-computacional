// =========================================================================
// @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
// =========================================================================

#include <map>
#include <queue>
#include <set>

#include <Eigen/Core>
#include <Eigen/Dense>

#include <CGAL/Cartesian.h>
#include <CGAL/Kernel_traits.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/draw_polyhedron.h>
#include <CGAL/draw_surface_mesh.h>

// -------------------------------------------------------------------------
template< class _M >
typename CGAL::Kernel_traits< typename _M::Point >::Kernel::Vector_3
surface_mesh_face_normal( const _M& m, const typename _M::Face_index& f )
{
  using _K = typename CGAL::Kernel_traits< typename _M::Point >::Kernel;

  typename _M::Halfedge_index h = m.halfedge( f );
  typename _K::Vector_3 p1 = m.point( m.source( h ) ) - CGAL::ORIGIN;
  typename _K::Vector_3 p2 = m.point( m.source( m.next( h ) ) ) - CGAL::ORIGIN;
  typename _K::Vector_3 p3 = m.point( m.target( m.next( h ) ) ) - CGAL::ORIGIN;
  typename _K::Vector_3 n;

  n  = CGAL::cross_product( p1, p2 );
  n += CGAL::cross_product( p2, p3 );
  n += CGAL::cross_product( p3, p1 );
  n /= std::sqrt( n.squared_length( ) );
  return( n );
}

// -------------------------------------------------------------------------
template< class _M >
typename CGAL::Kernel_traits< typename _M::Point >::Kernel::Point_3
surface_mesh_face_center( const _M& m, const typename _M::Face_index& f )
{
  using _K = typename CGAL::Kernel_traits< typename _M::Point >::Kernel;
  using _P = typename _K::Point_3;

  typename _M::Halfedge_index h = m.halfedge( f );
  typename _K::Vector_3 p;
  p  = m.point( m.source( h ) ) - CGAL::ORIGIN;
  p += m.point( m.source( m.next( h ) ) ) - CGAL::ORIGIN;
  p += m.point( m.target( m.next( h ) ) ) - CGAL::ORIGIN;

  return( _P( p[ 0 ] / 3.0, p[ 1 ] / 3.0, p[ 2 ] / 3.0 ) );
}

// -------------------------------------------------------------------------
template< class _M, class _S >
void build_simplex_mesh( _S& simplex, const _M& mesh )
{
  CGAL::Polyhedron_incremental_builder_3< typename _S::HalfedgeDS >
    builder( simplex.hds( ), true );
  builder.begin_surface( mesh.faces( ).size( ), mesh.vertices( ).size( ) );

  // Vertices
  std::map< typename _M::Face_index, unsigned long long > sx_verts;
  std::queue< typename _M::Face_index > qf;
  unsigned long long c = 0;
  qf.push( *( mesh.faces( ).begin( ) ) );
  while( qf.size( ) > 0 )
  {
    typename _M::Face_index f = qf.front( );
    qf.pop( );

    if( sx_verts.find( f ) == sx_verts.end( ) )
    {
      builder.add_vertex( surface_mesh_face_center( mesh, f ) );
      sx_verts[ f ] = c++;

      typename _M::Halfedge_index h = mesh.halfedge( f );
      qf.push( mesh.face( mesh.opposite( h ) ) );
      qf.push( mesh.face( mesh.opposite( mesh.next( h ) ) ) );
      qf.push( mesh.face( mesh.opposite( mesh.next( mesh.next( h ) ) ) ) );
    } // end if
  } // end while

  // Faces
  std::set< typename _M::Vertex_index > vm;
  std::queue< typename _M::Vertex_index > qv;
  qv.push( *( mesh.vertices( ).begin( ) ) );
  while( qv.size( ) > 0 )
  {
    typename _M::Vertex_index v = qv.front( );
    qv.pop( );

    if( vm.find( v ) == vm.end( ) )
    {
      vm.insert( v );

      typename _M::Halfedge_index h = mesh.opposite( mesh.halfedge( v ) );
      typename _M::Halfedge_index hIt = mesh.prev_around_source( h );

      builder.begin_facet( );

      while( hIt != h )
      {
        qv.push( mesh.target( hIt ) );
        builder.add_vertex_to_facet( sx_verts[ mesh.face( hIt ) ] );
        hIt = mesh.prev_around_source( hIt );
      } // end while
      qv.push( mesh.target( h ) );
      builder.add_vertex_to_facet( sx_verts[ mesh.face( h ) ] );

      builder.end_facet( );
    } // end if
  } // end while

  builder.end_surface( );
}

using TReal = float;

std::pair<Eigen::Matrix< TReal, 3, 1 >, TReal> circumcircle(
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
	return {cc.transpose(), rc};
}
std::pair<Eigen::Matrix< TReal, 3, 1 >, TReal> circumsphere(
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
	return { cs.transpose(), rs };
}
