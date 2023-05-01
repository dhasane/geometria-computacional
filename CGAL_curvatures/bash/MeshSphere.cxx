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
using TFace = TMesh::Face_index;

using TFunction = CGAL_EXT::Sphere_function< TKernel >;

class Topologia {
    std::map<int, TPoint> m;
	TMesh *tm;


public:
	Topologia(TMesh *m) {
		this->tm = m;
	}

	TMesh::face_index add_face(TMesh::vertex_index p1,
							   TMesh::vertex_index p2,
							   TMesh::vertex_index p3) {
		bool valid1 = tm->has_valid_index(p1);
		bool valid2 = tm->has_valid_index(p2);
		bool valid3 = tm->has_valid_index(p3);
		std::cout << "adding: " << std::endl
				  << "\t" << p3 << " is valid " << valid1 << std::endl
				  << "\t" << p2 << " is valid " << valid2 << std::endl
				  << "\t" << p1 << " is valid " << valid3 << std::endl;
		TMesh::face_index f = tm->add_face(p1, p2, p3);
		assert(f != TMesh::null_face());
		return f;
	}

    void get_topologia(std::vector<TMesh::vertex_index> puntos) {
        add_face(puntos[2], puntos[1], puntos[0]);
        add_face(puntos[0], puntos[1], puntos[2]);

        for (int a = 3; a < puntos.size(); a++) {
			std::cout << "started with " << a << std::endl;
            TMesh::face_index face = closest_face(puntos[a]);

			std::cout << "closest face index " << face << std::endl;

            replace_face(face, puntos[a]);
			return;
        }
    }

	void print_all_faces() {
        TMesh::face_iterator vb, ve;
		std::cout << " all faces : " << std::endl;
        for (boost::tie(vb, ve) = faces(*tm); vb != ve; ++vb) {
			TMesh::Halfedge_index hf = tm->halfedge(*vb);
			TMesh::Face_index fi = tm->face(hf);
			std::cout << fi << " ";
        }
		std::cout << std::endl;
	}

private:
    TMesh::face_index closest_face(TMesh::vertex_index vertex_index) {
        TMesh::face_iterator vb, ve;

		TPoint vertex = tm->point(vertex_index);

		double min_dist = 0;
		TMesh::Face_index min_face_index;

		bool first = true;

        for (boost::tie(vb, ve) = faces(*tm); vb != ve; ++vb) {
            // TMesh::Face_index face_index = tm.face(tm.halfedge(*vb));
            // tm.face(face);

			TMesh::Halfedge_index hf = tm->halfedge(*vb);

			// en caso de que la cara aun no este presente en el mapa
			// evitar calcular esto multiples veces
			TMesh::Face_index fi = tm->face(hf);
			if (m.count(fi) == 0) {
				double x;
				double y;
				double z;
				for (TMesh::Halfedge_index hi : halfedges_around_face(hf, *tm)) {
					TMesh::Vertex_index vi = target(hi, *tm);
					TPoint p = tm->point(vi);
					x += p.hx();
					y += p.hy();
					z += p.hz();
				}

				m[fi] = TPoint{x,y,z};
			}

			double dist = squared_distance(m[fi], vertex);
			if (first || dist <= min_dist) {
				min_dist = dist;
				min_face_index = fi;

				first = false;
			}
        }
		return min_face_index;
    }

    void replace_face(TMesh::face_index face, TMesh::vertex_index pt) {
        CGAL::Vertex_around_face_iterator<TMesh> vbegin, vend;
        boost::tie(vbegin, vend) = vertices_around_face(tm->halfedge(face), *tm);
	
	Tmesh::vertex_index first_vertex, prev_vertex;
	   first_vertex = prev_vertex = *vbegin;
	    ++vbegin:
	    std::cout<< "pre add ";
	    print_all_faces();
	    for(; vbegin != vend; ++vbegin) {
	    	add_face(pt,prev_vertex, *vbegin);
		prev_vertex = *vbegin;
	    }
	    add_face(pt, prev_vertex, first_vertex);
        //auto fit = vbegin++;

	//	std::cout << " pre add ";
	//	print_all_faces();
      //  for (; fit != vend; ++fit) {
			// TODO aqui hay un error
	//		add_face(pt, *std::next(fit, -1), *fit);
       // }

                std::cout << " pre remove ";
		print_all_faces();

		m.erase(face);
        tm->remove_face(face);
		std::cout << "face removed from map " << face << std::endl;
		print_all_faces();
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
  Topologia t(&m);

  t.get_topologia(puntos);

  // Save results
  CGAL::IO::write_OBJ( "mesh.obj", m );
  return( EXIT_SUCCESS );
}

// eof - $RCSfile$
