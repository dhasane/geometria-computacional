// =========================================================================
// @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
// =========================================================================

#include <functional>
#include <iostream>

#include <CGAL/Cartesian.h>
#include <CGAL/Surface_mesh.h>

#include <CGAL_EXT/Sphere_function.h>

// -------------------------------------------------------------------------
// Types
using TKernel = CGAL::Cartesian< double >;
using TReal = TKernel::FT;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TFace = CGAL::Surface_mesh< TPoint >::face;

using TFunction = CGAL_EXT::Sphere_function< TKernel >;

class Topologia {

    TMesh::face_index closest_face(TMesh tm, TMesh::vertex_index vertex) {
        TMesh::face_iterator vb, ve;
        for (boost::tie(vb, ve) = faces(tm); vb != ve; ++vb) {
			// TODO esto aun no sirve
			TFace face = tm.face(vb)
            std::cout << "        " << tm.face(vb) << "\n";
        }
    }

    void replace_face(TMesh tm, TMesh::face_index face, TMesh::vertex_index pt) {
        CGAL::Vertex_around_face_iterator<TMesh> vbegin, vend;
        boost::tie(vbegin, vend) = vertices_around_face(tm.halfedge(face), tm);
        auto fit = vbegin++;

        for (; fit != vend--; ++fit) {
            tm.add_face(pt, *fit--, *fit);
        }

        tm.remove_face(face);
    }

    void get_topologia(TMesh tm, std::vector<TMesh::vertex_index> puntos) {
        tm.add_face(puntos[0], puntos[1], puntos[2]);
        tm.add_face(puntos[2], puntos[1], puntos[0]);

        for (int a = 3; a < puntos.size(); a++) {
            TMesh::face_index face = closest_face(tm, puntos[a]);

            replace_face(tm, face, puntos[a]);
        }
    }
};

// -------------------------------------------------------------------------
int main( int argc, char** argv )
{
  // Properties
  TReal r = 3;
  unsigned int nU = 5;
  unsigned int nV = 5;
  TReal minV = 0;
  TReal maxV = TReal( 4 ) * TReal( std::atan( double( 1 ) ) );
  TReal minU = 0;
  TReal maxU = maxV * TReal( 2 );
  bool closedU = true;
  bool closedV = false;

  // Base objects
  TMesh m;
  TFunction f;
  f.set_radius( r );

  std::vector<TMesh::Vertex_index> puntos;

  // Geometry
  for( unsigned int iU = 0; iU < nU; ++iU )
  {
    TReal u = TReal( iU ) / TReal( nU - ( ( closedU )? 0: 1 ) );
    u = ( u * ( maxU - minU ) ) + minU;
    for( unsigned int iV = 0; iV < nV; ++iV )
    {
      TReal v = TReal( iV ) / TReal( nV - ( ( closedV )? 0: 1 ) );
      v = ( v * ( maxV - minV ) ) + minV;

      TPoint punto =  f.evaluate( u, v );

      puntos.push_back( m.add_vertex(punto) );
    } // end for
  } // end for

  // Topology

  // TODO aqui seria llamar lo de arriba

  // Save results
  CGAL::IO::write_OBJ( "mesh.obj", m );
  return( EXIT_SUCCESS );
}

// eof - $RCSfile$
