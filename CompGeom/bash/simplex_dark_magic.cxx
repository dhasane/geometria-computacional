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
