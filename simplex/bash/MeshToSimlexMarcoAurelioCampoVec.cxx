// =========================================================================
// @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
// =========================================================================

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

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
// Types
using TKernel = CGAL::Cartesian< float >;
using TReal   = TKernel::FT;
using TPoint  = TKernel::Point_3;
using TMesh   = CGAL::Surface_mesh< TPoint >;
using TSimplex = CGAL::Polyhedron_3< TKernel >;

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
//REVISAR

//FUNCIONES MARCO AURELIANO Y CAMPO VECTORIAL:
//Los puntos de la malla con Marco Aurelio
void adjust_points_marco_aurelio( _S& simplex, const typename  _S::Facet_handle& fh)
{
 
    TPoint& p1 = fh->first->vertex(fh->second)->point();
    TPoint& p2 = fh->first->vertex(fh->third)->point();
    TPoint& p3 = fh->first->vertex(fh->fourth)->point();

   
    TPoint mid_point = (p2 + p3) / 2.0;

    //Esta es la parte donde se diferencia con CV
    // Proyectar el pmedio sobre la esfera
    TPoint new_point = CGAL::ORIGIN + (mid_point - CGAL::ORIGIN) / CGAL::sqrt(mid_point.squared_length());
    p1 = new_point;
}

//Los puntos de la malla con Campo Vectorial
void adjust_points_campo_vectorial( _S& simplex, const typename  _S::Facet_handle& fh)
{
    
    TPoint& p1 = fh->first->vertex(fh->second)->point();
    TPoint& p2 = fh->first->vertex(fh->third)->point();
    TPoint& p3 = fh->first->vertex(fh->fourth)->point();

    TPoint mid_point = (p2 + p3) / 2.0;
    //Esta es la parte donde se diferencia con MA
    // vector normal desde el centro hasta el p medio
    TKernel::Vector_3 normal = mid_point - CGAL::ORIGIN;
    normal = normal / CGAL::sqrt(normal.squared_length());

    
    TPoint new_point = CGAL::ORIGIN + normal;
    p1 = new_point;
}

// Ajustar los puntos de la malla para que sea lo mas equilartero y mantener la geometria/topologia de la esfera
void adjust_points_and_maintain_sphere( _S& simplex, const typename  _S::Facet_handle& fh, const std::string& method)
{
    if (method == "MA")
    {
        adjust_points_marco_aurelio(simplex, fh);
    }
    else if (method == "CV")
    {
        adjust_points_campo_vectorial(simplex, fh);
    }
    else
    {
        //fuckoffdude
        return;
    }

    // recalcula el centro de la circunsphere con los ajustes ya sea de MA o CV 
    TPoint cs_center = simplex_tetrahedron_circumcenter(simplex, fh);
    // Vertices con el centro ajustado
    fh->first->vertex(fh->third)->point() = cs_center;
    fh->first->vertex(fh->fourth)->point() = cs_center;
}

// -------------------------------------------------------------------------
int main( int argc, char** argv )
{
  TMesh mesh;
  TSimplex simplex;

 // CGAL::IO::read_STL( argv[ 1 ], mesh );
  CGAL::read_STL("mesh.stl", mesh);//No estoy seguro aca no recuerdo si dijo que era un .obj 
  build_simplex_mesh( simplex, mesh );

  for(
    TSimplex::Vertex_iterator vIt = simplex.vertices_begin( );
    vIt != simplex.vertices_end( );
    ++vIt
    )
  {
    TSimplex::Halfedge_handle h1 = vIt->halfedge( );
    TSimplex::Halfedge_handle h2 = h1->opposite( )->prev( );
    TSimplex::Halfedge_handle h3 = h2->opposite( )->prev( );

    // Neighboring points
    TKernel::Vector_3 p = h1->vertex( )->point( ) - CGAL::ORIGIN;
    TKernel::Vector_3 p1 = h1->opposite( )->vertex( )->point( ) - CGAL::ORIGIN;
    TKernel::Vector_3 p2 = h2->opposite( )->vertex( )->point( ) - CGAL::ORIGIN;
    TKernel::Vector_3 p3 = h3->opposite( )->vertex( )->point( ) - CGAL::ORIGIN;

    // Normal
    TKernel::Vector_3 n = CGAL::cross_product( p1, p2 );
    n += CGAL::cross_product( p2, p3 );
    n += CGAL::cross_product( p3, p1 );
    n /= std::sqrt( n.squared_length( ) );

    // Transformation to XY plane
    Eigen::Matrix< TReal, 3, 3 > Rxy;
    Eigen::Matrix< TReal, 3, 1 > v, v1, v2, v3, v2_2d, v3_2d;
    v << p[ 0 ], p[ 1 ], p[ 2 ];
    v1 << p1[ 0 ], p1[ 1 ], p1[ 2 ];
    v2 << p2[ 0 ], p2[ 1 ], p2[ 2 ];
    v3 << p3[ 0 ], p3[ 1 ], p3[ 2 ];

    TReal ct = n[ 2 ];
    TReal st = std::sqrt( ( n[ 0 ] * n[ 0 ] ) + ( n[ 1 ] * n[ 1 ] ) );
    TReal u1 = n[ 1 ] / st;
    TReal u2 = -n[ 0 ] / st;

    Rxy( 0, 0 ) = ct + ( u1 * u1 * ( 1.0 - ct ) );
    Rxy( 1, 1 ) = ct + ( u2 * u2 * ( 1.0 - ct ) );
    Rxy( 2, 2 ) = ct;
    Rxy( 0, 1 ) = u1 * u2 * ( 1.0 - ct );
    Rxy( 1, 0 ) = u1 * u2 * ( 1.0 - ct );
    Rxy( 0, 2 ) = u2 * st;
    Rxy( 2, 0 ) = -u2 * st;
    Rxy( 1, 2 ) = -u1 * st;
    Rxy( 2, 1 ) = u1 * st;

    v2_2d = Rxy * ( v2 - v1 );
    v3_2d = Rxy * ( v3 - v1 );

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

    std::cout << cc.transpose( ) << " : ";
    std::cout << rc << std::endl;

    // Circumsphere
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

    std::cout << cs.transpose( ) << " : " << rs << std::endl;
  } // end for
  // PROBAR!
   
  for (auto fh = simplex.facets_begin(); fh != simplex.facets_end(); ++fh)
  {
      // Centro del circuncirculo
      TPoint cc_center = surface_mesh_face_circumcenter(mesh, fh->first->index());
      // Centro de la circunsphere
      TPoint cs_center = simplex_tetrahedron_circumcenter(simplex, fh);

      adjust_points_and_maintain_sphere(simplex, fh, "MA");
      //adjust_points_and_maintain_sphere(simplex, fh, "CV"); 
     

      fh->first->vertex(fh->second)->point() = cc_center;
      fh->first->vertex(fh->third)->point() = cs_center;
      fh->first->vertex(fh->fourth)->point() = cs_center;
  }

  CGAL::draw( simplex );

  return( EXIT_SUCCESS );
}

// eof - $RCSfile$
