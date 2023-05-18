#ifndef LAB_TO_MESH
#define LAB_TO_MESH

#include <CGAL/Surface_mesh.h>
#include <CGAL/Cartesian.h>

#include "pos.cxx"
#include "labyrinth.cxx"

using TKernel = CGAL::Cartesian< double >;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TVertex = TMesh::vertex_index;

class LabToMesh {
public:
    static TVertex vertex_in_pos(TVertex ***mat, Pos p) {
		// std::cout << p.to_string() << std::endl;
        return mat[p.x][p.y][p.z];
    }

    static void wall_x (TMesh& m, Pos p, TVertex ***mat, bool front = true) {
		TMesh::face_index f;
		if (front) {
			f = m.add_face(
				vertex_in_pos(mat, p.add_pos({1, 0, 0})),
				vertex_in_pos(mat, p.add_pos({1, 1, 0})),
				vertex_in_pos(mat, p.add_pos({1, 1, 1})),
				vertex_in_pos(mat, p.add_pos({1, 0, 1})));
		} else {
			f = m.add_face(
				vertex_in_pos(mat, p.add_pos({0, 0, 0})),
				vertex_in_pos(mat, p.add_pos({0, 0, 1})),
				vertex_in_pos(mat, p.add_pos({0, 1, 1})),
				vertex_in_pos(mat, p.add_pos({0, 1, 0})));
		}
		assert(f != TMesh::null_face());
    };
    static void wall_y (TMesh& m, Pos p, TVertex ***mat, bool front = true) {
		TMesh::face_index f;
		if (front) {
			f = m.add_face(
				vertex_in_pos(mat, p.add_pos({0, 1, 0})),
				vertex_in_pos(mat, p.add_pos({0, 1, 1})),
				vertex_in_pos(mat, p.add_pos({1, 1, 1})),
				vertex_in_pos(mat, p.add_pos({1, 1, 0})));
		} else {
			f = m.add_face(
				vertex_in_pos(mat, p.add_pos({0, 0, 0})),
				vertex_in_pos(mat, p.add_pos({1, 0, 0})),
				vertex_in_pos(mat, p.add_pos({1, 0, 1})),
				vertex_in_pos(mat, p.add_pos({0, 0, 1})));
		}
		assert(f != TMesh::null_face());
    };
    static void wall_z (TMesh& m, Pos p, TVertex ***mat, bool front = true) {
		TMesh::face_index f;
		if (front) {
			f = m.add_face(
				vertex_in_pos(mat, p.add_pos({0, 0, 1})),
				vertex_in_pos(mat, p.add_pos({1, 0, 1})),
				vertex_in_pos(mat, p.add_pos({1, 1, 1})),
				vertex_in_pos(mat, p.add_pos({0, 1, 1})));
		} else {
			f = m.add_face(
				vertex_in_pos(mat, p.add_pos({0, 0, 0})),
				vertex_in_pos(mat, p.add_pos({0, 1, 0})),
				vertex_in_pos(mat, p.add_pos({1, 1, 0})),
				vertex_in_pos(mat, p.add_pos({1, 0, 0})));
		}
		assert(f != TMesh::null_face());
    };

    static void path_x (TMesh& m, Pos p, TVertex ***mat, bool front = true) {

		int dir = front ? 1 : -1;
	// y
	// b
		m.add_face(vertex_in_pos(mat, p.add_pos({dir + 0, 0, 0})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 0, 0})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 0, 1})),
				   vertex_in_pos(mat, p.add_pos({dir + 0, 0, 1})));

	// f
		m.add_face(vertex_in_pos(mat, p.add_pos({dir + 0, 1, 0})),
				   vertex_in_pos(mat, p.add_pos({dir + 0, 1, 1})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 1, 1})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 1, 0}))) ;

	// z
	// b
		m.add_face(vertex_in_pos(mat, p.add_pos({dir + 0, 0, 0})),
				   vertex_in_pos(mat, p.add_pos({dir + 0, 1, 0})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 1, 0})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 0, 0})));

	// f
		m.add_face(vertex_in_pos(mat, p.add_pos({dir + 0, 0, 1})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 0, 1})),
				   vertex_in_pos(mat, p.add_pos({dir + 1, 1, 1})),
				   vertex_in_pos(mat, p.add_pos({dir + 0, 1, 1})));

    };
    static void path_y (TMesh& m, Pos p, TVertex ***mat, bool front = true) {
		int dir = front ? 1 : -1;
	// x
	// b
		m.add_face(vertex_in_pos(mat, p.add_pos({0, dir + 0, 0})),
				   vertex_in_pos(mat, p.add_pos({0, dir + 0, 1})),
				   vertex_in_pos(mat, p.add_pos({0, dir + 1, 1})),
				   vertex_in_pos(mat, p.add_pos({0, dir + 1, 0})));

	// f
		m.add_face(vertex_in_pos(mat, p.add_pos({1, dir + 0, 0})),
				   vertex_in_pos(mat, p.add_pos({1, dir + 1, 0})),
				   vertex_in_pos(mat, p.add_pos({1, dir + 1, 1})),
				   vertex_in_pos(mat, p.add_pos({1, dir + 0, 1})));

	// z
	// b
		m.add_face(vertex_in_pos(mat, p.add_pos({0, dir + 0, 0})),
				   vertex_in_pos(mat, p.add_pos({0, dir + 1, 0})),
				   vertex_in_pos(mat, p.add_pos({1, dir + 1, 0})),
				   vertex_in_pos(mat, p.add_pos({1, dir + 0, 0})));

	// f
		m.add_face(vertex_in_pos(mat, p.add_pos({0, dir + 0, 1})),
				   vertex_in_pos(mat, p.add_pos({1, dir + 0, 1})),
				   vertex_in_pos(mat, p.add_pos({1, dir + 1, 1})),
				   vertex_in_pos(mat, p.add_pos({0, dir + 1, 1})));

    };
    static void path_z (TMesh& m, Pos p, TVertex ***mat, bool front = true) {
		int dir = !front ? 1 : -1;
	// x
	// b
		m.add_face(vertex_in_pos(mat, p.add_pos({0, 0, dir + 0})),
				   vertex_in_pos(mat, p.add_pos({0, 0, dir + 1})),
				   vertex_in_pos(mat, p.add_pos({0, 1, dir + 1})),
				   vertex_in_pos(mat, p.add_pos({0, 1, dir + 0})));

	// f
		m.add_face(vertex_in_pos(mat, p.add_pos({1, 0, dir + 0})),
				   vertex_in_pos(mat, p.add_pos({1, 1, dir + 0})),
				   vertex_in_pos(mat, p.add_pos({1, 1, dir + 1})),
				   vertex_in_pos(mat, p.add_pos({1, 0, dir + 1})));

	// y
	// b
		m.add_face(vertex_in_pos(mat, p.add_pos({0, 0, dir + 0})),
				   vertex_in_pos(mat, p.add_pos({1, 0, dir + 0})),
				   vertex_in_pos(mat, p.add_pos({1, 0, dir + 1})),
				   vertex_in_pos(mat, p.add_pos({0, 0, dir + 1})));

	// f
		m.add_face(vertex_in_pos(mat, p.add_pos({0, 1, dir + 0})),
				   vertex_in_pos(mat, p.add_pos({0, 1, dir + 1})),
				   vertex_in_pos(mat, p.add_pos({1, 1, dir + 1})),
				   vertex_in_pos(mat, p.add_pos({1, 1, dir + 0}))) ;

    };

    static void pos_to_box(TMesh &m, Labyrinth l, Pos p, TVertex ***points) {
        Room *r = l.get_room(p);

		// todos los posibles movimientos
		Pos p1{p.x + 1, p.y    , p.z    };
		Pos p2{p.x - 1, p.y    , p.z    };
		Pos p3{p.x    , p.y + 1, p.z    };
		Pos p4{p.x    , p.y - 1, p.z    };
		Pos p5{p.x    , p.y    , p.z + 1};
		Pos p6{p.x    , p.y    , p.z - 1};

		bool forward = false;
		bool back = false;
		bool up = false;
		bool down = false;
		bool left = false;
		bool right = false;

		for (Pos w: r->all_paths()) {
			if (p1 == w) {
				forward = true;
			}
			else if (p2 == w) {
				back = true;
			}
			else if (p3 == w) {
				left = true;
			}
			else if (p4 == w) {
				right = true;
			}
			else if (p5 == w) {
				down = true;
			}
			else if (p6 == w) {
				up = true;
			}
		}
		auto real_pos = [](Pos p) -> Pos{
			// esto se hace para que cuartos adyacentes no compartan sus paredes
			// pos : 1,      2,      3,      4,      5
			// real: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5
			return (Pos){p.x * 2, p.y * 2, p.z * 2};
		};

		// los path_ solo se necesitan en una direccion

		if (forward) {path_x(m, real_pos(p), points, true);}
		else	     {wall_x(m, real_pos(p), points, true);}
		if (back)    {/*path_x(m, real_pos(p), points, false);*/}
		else         {wall_x(m, real_pos(p), points, false);}

		if (up)      {/*path_z(m, real_pos(p), points, true);*/}
		else         {wall_z(m, real_pos(p), points, false);}
		if (down)    {path_z(m, real_pos(p), points, false);}
		else         {wall_z(m, real_pos(p), points, true);}

		if (left)    {/*path_y(m, real_pos(p), points, true);*/}
		else         {wall_y(m, real_pos(p), points, true);}
		if (right)   {path_y(m, real_pos(p), points, false);}
		else         {wall_y(m, real_pos(p), points, false);}
	}

	static TMesh to_obj(Labyrinth l) {
		TVertex ***points;

		TMesh m;

		int base_sep = 5;

		// el + 2 es para tener en cuenta las paredes externas
		// el * 2, es para que haya dos paredes por cuarto, a cada lado
		int points_x = (l.max.x * 2) + 2;
		int points_y = (l.max.y * 2) + 2;
		int points_z = (l.max.z * 2) + 2;

		points = new TVertex **[points_x];
		for (int x = 0; x < points_x; ++x) {
			points[x] = new TVertex *[points_y];
			for (int y = 0; y < points_y; ++y) {
				points[x][y] = new TVertex[points_z];
				for (int z = 0; z < points_z; ++z) {
					points[x][y][z] = m.add_vertex({x, y, z});
                }
            }
        }

		for (Pos p : l.get_path()) {
			pos_to_box(m, l, p, points);
		}

		// TODO retornar cara en entrada y cara en salida
        return m;
    }
};

#endif
