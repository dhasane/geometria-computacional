#include <asm-generic/errno.h>
#include <iostream>
#include <vector>
#include <deque>
#include <random>
#include <tuple>
#include <sstream>
#include <string>
#include <algorithm>

// CGAL
#include <CGAL/Surface_mesh.h>
#include <CGAL/Cartesian.h>

std::random_device r;
std::default_random_engine gen{r()};
std::uniform_real_distribution<> dis{0, 1};

using TKernel = CGAL::Cartesian< double >;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TVertex = TMesh::vertex_index;

int get_random(int to) {
	std::uniform_int_distribution<> dis{0, to - 1};
	return dis(gen);
}

class Pos {
public:

	int x;
	int y;
	int z;

	std::string to_string() {
		std::ostringstream s;
		s << "("
		<< x << ", "
		<< y << ", "
		<< z << ")";
		return s.str();
	}

	bool operator==(Pos p2) {
		return
			x == p2.x &&
			y == p2.y &&
			z == p2.z;
	}

	Pos add_pos(Pos p) {
		return (Pos){x+p.x, y+p.y, z+p.z};
	}
};

class Room {
	bool filled;

public:
	Pos from;
	std::vector<Pos> path;

	std::vector<Pos> get_connected() {
		return path;
	}

	std::vector<Pos> all_paths() {
		std::vector<Pos> all_paths{path};
		all_paths.push_back(from);
		return all_paths;
	}

	Room() {
		filled = false;
	}

	bool is_filled() { return filled; }

	void fill() {
		filled = true;
	}

	void connect(Pos c) {
		path.push_back(c);
	}
};

#define OUTSIDE {-2, -2, -2}

class Labyrinth {

	Room ***rooms;
	Pos start;
	Pos end;

	Pos max;

	bool complete;

	//               <actual, desde>
	std::deque<std::pair<Pos, Pos>> potential_rooms;

public:
	Labyrinth() {
		max = {2, 2, 2};
		rooms = new Room **[max.x];
		for (int i = 0; i < max.x; ++i) {
			rooms[i] = new Room *[max.y];
			for (int j = 0; j < max.y; ++j) {
				rooms[i][j] = new Room[max.z];
			}
		}

		Pos f{0, 0, 0};
		start = f;
	}
	Labyrinth(int size_x, int size_y, int size_z) {
		max = {size_x, size_y, size_z};
		complete = false;

		// matrix cuadrada
		rooms = new Room **[max.x];
		for (int i = 0; i < max.x; ++i) {
			rooms[i] = new Room *[max.y];
			for (int j = 0; j < max.y; ++j) {
				rooms[i][j] = new Room[max.z];
			}
		}

		// siempre empieza en la capa 0 de z, por lo tanto, termina al llegar a max_z
		Pos f{get_random(max.x), get_random(max.y), 0};
		start = f;
		potential_rooms.push_back({f, OUTSIDE});

		build();
	}

	void sub_paths(Pos p, int depth) {
		for (Pos a : get_room(p)->path) {
			print_sub_paths(a, depth + 1);
		}
	}

	std::vector<Pos> get_path() {
		std::deque<Pos> dest;
		std::vector<Pos> path;

		for (auto a : get_room(start)->path) {
			dest.push_back(a);
			path.push_back(a);
		}

		while(!dest.empty()) {
			Pos cur = dest.front();
			dest.pop_front();

			for (auto a : get_room(cur)->path) {
				dest.push_back(a);
                path.push_back(a);
			}
		}

		std::cout << "path: ";
		print_rooms(path);
		std::cout << std::endl;
		return path;
	}

	void print_sub_paths(Pos p, int depth) {
		for (int a = 0; a < depth; a++) {
			std::cout << "-";
		}
		std::cout << p.to_string();
		if (p == end) {
			std::cout << " - final " ;
		}
		std::cout << std::endl;

		for (Pos a : get_room(p)->path) {
			print_sub_paths(a, depth + 1);
		}
	}

	void print_paths() {
		std::deque<std::pair<Pos, int>> dest;

		std::cout << "start: " << start.to_string() << std::endl;

		print_sub_paths(start, 0);
	}

	void print_potential_rooms () {
		std::cout << "rooms: ";
		for (auto p : potential_rooms) {
			std::cout << p.first.to_string();
		}
	}

	static void print_rooms (std::vector<Pos> rooms) {
		for (Pos p : rooms) {
			std::cout << p.to_string();
		}
	}

	void build() {
		while (!potential_rooms.empty()) {
			Pos current;
			Pos prev;

			// std::shuffle( std::begin( potential_rooms ) , std::end( potential_rooms ) , gen );

			std::tie(current, prev) = potential_rooms.front();

			potential_rooms.pop_front();

			Room *r = get_room(current);

			if (!r->is_filled()) {
				// std::cout << std::endl;
				// std::cout << std::endl;
				// std::cout << "from " << prev.to_string() << " to " << current.to_string() << std::endl;
				r->from = prev;
				if (0 <= prev.x) {
					get_room(prev)->connect(current);
				}
				r->fill();

				Pos next;
				std::vector<Pos> not_selected;

                if (select_random_room(current, next, not_selected)) {
                    potential_rooms.push_front({next, current});

                    // std::cout << "next: " << next.string_pos() << std::endl;
                    // std::cout << "not selected: ";
                    // print_rooms(not_selected);

                    for (Pos p: not_selected) {
                        potential_rooms.push_back({p, current});
                    }
                }
                // for (Pos p: surrounding(current)) {
                //  potential_rooms.push_back({p, current});
                // }
			}
		}
	}

	static std::vector<Pos> get_walls(Pos p, std::vector<Pos> paths) {
		std::vector<Pos> possible_walls;
		std::vector<Pos> walls;
		int x = p.x;
		int y = p.y;
		int z = p.z;

		possible_walls.push_back({x + 1, y, z});
		possible_walls.push_back({x - 1, y, z});
		possible_walls.push_back({x, y + 1, z});
		possible_walls.push_back({x, y - 1, z});
		possible_walls.push_back({x, y, z + 1});
		possible_walls.push_back({x, y, z - 1});

		auto not_present = [&paths](const Pos& key) -> bool
			{
				return std::find(
					paths.begin(), paths.end(), key) == paths.end();
			};
		std::copy_if (
			possible_walls.begin(), possible_walls.end(),
			std::back_inserter(walls), not_present);

		std::cout << "surrounding : ";
		print_rooms(possible_walls);

		std::cout << std::endl << "path : ";
		print_rooms(paths);

		std::cout << std::endl << "walls : ";
		print_rooms(walls);
		std::cout << std::endl;

        return walls;
    }

    static TVertex vertex_in_pos(TVertex ***mat, Pos p) {
		std::cout << p.to_string() << std::endl;
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

    static void pos_to_box(TMesh &m, Labyrinth l, Pos p, TVertex ***points) {
        Room *r = l.get_room(p);

        std::vector<Pos> walls = get_walls(p, r->all_paths());

		// todos los posibles movimientos
		Pos p1{p.x + 1, p.y    , p.z    };
		Pos p2{p.x - 1, p.y    , p.z    };
		Pos p3{p.x    , p.y + 1, p.z    };
		Pos p4{p.x    , p.y - 1, p.z    };
		Pos p5{p.x    , p.y    , p.z + 1};
		Pos p6{p.x    , p.y    , p.z - 1};

		// std::cout << "direcciones :" << p1.to_string()
		// 		  << p1.to_string()
		// 		  << p2.to_string()
		// 		  << p3.to_string()
		// 		  << p4.to_string()
		// 		  << p5.to_string()
		// 		  << p6.to_string() ;
		// std::cout << std::endl << "paredes :";
		// print_rooms(walls);
		// std::cout << std::endl;

		auto real_pos = [](Pos p) -> Pos{
			// esto se hace para que cuartos adyacentes no compartan sus paredes
			// pos : 1,      2,      3,      4,      5
			// real: 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5
			return (Pos){p.x * 2, p.y * 2, p.z * 2};
		};

		for (Pos w: walls) {
			std::cout << " - from " << p.to_string() << " " << w.to_string() << std::endl ;
			if (p1 == w) {
				std::cout << "equal to " << p1.to_string() <<std::endl;
				wall_x(m, real_pos(p), points, true);
			}
			else if (p2 == w) {
				std::cout << "equal to " << p1.to_string() <<std::endl;
				wall_x(m, real_pos(p), points, false);
			}
			else if (p3 == w) {
				std::cout << "equal to " << p1.to_string() <<std::endl;
				wall_y(m, real_pos(p), points, true);
			}
			else if (p4 == w) {
				std::cout << "equal to " << p1.to_string() <<std::endl;
				wall_y(m, real_pos(p), points, false);
			}
			else if (p5 == w) {
				std::cout << "equal to " << p1.to_string() <<std::endl;
				wall_z(m, real_pos(p), points, true);
			}
			else if (p6 == w) {
				std::cout << "equal to " << p1.to_string() <<std::endl;
				wall_z(m, real_pos(p), points, false);
			}
		}
		std::cout << std::endl ;
	}

	TMesh to_obj(Labyrinth l) {

		TVertex ***points;

		TMesh m;

		int base_sep = 5;

		// el + 2 es para tener en cuenta las paredes externas
		// el * 2, es para que haya dos paredes por cuarto, a cada lado
		int points_x = (max.x * 2) + 2;
		int points_y = (max.y * 2) + 2;
		int points_z = (max.z * 2) + 2;

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

        // TODO: algun dia optimizar esto, que de momento esta un asco
        // for (int x = 0; x < max.x; ++x) {
        //     for (int y = 0; y < max.y; ++y) {
        //         for (int z = 0; z < max.z; ++z) {
        //             pos_to_box(m, l, {x,y,z}, points);
        //         }
        //     }
        // }
        // for (Pos p: get_path()) {
        //  pos_to_box(m, p, points);
        // }
		for (Pos p : l.get_path()) {
			pos_to_box(m, l, p, points);
		}
        return m;
    }

    static TMesh test_box(Labyrinth l) {
        TVertex ***points;

        TMesh m;

        int base_sep = 5;

        // el + 2 es para tener en cuenta las paredes externas
        // el * 2, es para que haya dos paredes por cuarto, a cada lado
        int points_x = 1 * 2;
        int points_y = 1 * 2;
        int points_z = 1 * 2;

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

        pos_to_box(m, l, {0, 0, 0}, points);
		return m;
	}

	static TMesh test_to_obj(Labyrinth l) {
		TVertex ***points;

		TMesh m;
		int base_sep = 5;

		int max_x = 1;
		int max_y = 1;
		int max_z = 1;

		// el + 1 es para tener en cuenta las paredes externas
		points = new TVertex **[max_x + 2];
		for (int x = 0; x < max_x + 2; ++x) {
			points[x] = new TVertex *[max_y + 2];
			for (int y = 0; y < max_y + 2; ++y) {
				points[x][y] = new TVertex[max_z + 2];
				for (int z = 0; z < max_z + 2; ++z) {
					points[x][y][z] = m.add_vertex({x,y,z});
				}
			}
		}

		// TODO: algun dia optimizar esto, que de momento esta un asco
		// for (int x = 0; x < max_x; ++x) {
		// 	for (int y = 0; y < max_y; ++y) {
		// 		for (int z = 0; z < max_z; ++z) {
		// 			pos_to_box(m, l, {x,y,z}, points);
		// 		}
		// 	}
		// }
		for (Pos p : l.get_path()) {
			pos_to_box(m, l, p, points);
		}
		return m;
	}

private:

	bool verify_internal(Pos p) {
		return
			0 <= p.x && p.x < max.x &&
			0 <= p.y && p.y < max.y &&
			0 <= p.z && p.z < max.z;
	}

	Room* get_room(Pos p) {
		if (!verify_internal(p))
			return nullptr;

		int x = (p.x);
		int y = (p.y);
		int z = (p.z);

		return &rooms[x][y][z];
	}

	bool fill_room(std::vector<Pos> &surr, Pos from, Pos p) {
		int x = (p.x);
		int y = (p.y);
		int z = (p.z);

		if (0 <= x && x < max.x &&
			0 <= y && y < max.y
			)
		{
			// ya que llegar a max.z la primera vez es llegar a una "salida"
			if(0 <= z && z < max.z)
			{
				if (!rooms[x][y][z].is_filled()) {
					surr.push_back(p);
				}
			}
			else if (!complete && z >= max.z) {
				// para definir la puerta de salida
				end = from;
				complete = true;
				return true;
			}
		}
		return false;
	}

	std::vector<Pos> surrounding(Pos p) {
		std::vector<Pos> surr;

		int x = p.x;
		int y = p.y;
		int z = p.z;

		// en caso de encontrar el final, no quiero que tome en cuenta las casillas alrededor
		if (fill_room(surr, p, {x + 1, y, z}) ||
			fill_room(surr, p, {x - 1, y, z}) ||
			fill_room(surr, p, {x, y + 1, z}) ||
			fill_room(surr, p, {x, y - 1, z}) ||
			fill_room(surr, p, {x, y, z + 1}) ||
			fill_room(surr, p, {x, y, z - 1})) {

			surr.clear();
			std::cout << "final encontrado " << p.to_string() << std::endl;
		}

		return surr;
	}

	bool select_random_room(Pos current, Pos &next, std::vector<Pos>& not_selected) {
		std::vector<Pos> surr = surrounding(current);

		// std::cout << "posibles: " ;
		// print_rooms(not_selected);
		// std::cout << std::endl;
		// no es lo mas eficiente, pero quiero dejar la idea

		if (surr.empty()) {
			return false;
		}

		int sel = get_random(surr.size());

		for (int a = 0; a < surr.size(); a ++) {
			if (a == sel) {
				next = surr[a];
			}
			else {
				not_selected.push_back(surr[a]);
			}
		}

		return true;
	}
};


// TODO usar esto de referencia para armar el laberinto
TMesh test_box() {
	TMesh mesh;
	TMesh::Vertex_index v_0_0_0 = mesh.add_vertex(TPoint(0, 0, 0));
	TMesh::Vertex_index v_0_0_1 = mesh.add_vertex(TPoint(0, 0, 1));
	TMesh::Vertex_index v_0_1_0 = mesh.add_vertex(TPoint(0, 1, 0));
	TMesh::Vertex_index v_0_1_1 = mesh.add_vertex(TPoint(0, 1, 1));
	TMesh::Vertex_index v_1_0_0 = mesh.add_vertex(TPoint(1, 0, 0));
	TMesh::Vertex_index v_1_0_1 = mesh.add_vertex(TPoint(1, 0, 1));
	TMesh::Vertex_index v_1_1_0 = mesh.add_vertex(TPoint(1, 1, 0));
	TMesh::Vertex_index v_1_1_1 = mesh.add_vertex(TPoint(1, 1, 1));

	// x
	// b
	mesh.add_face(v_0_0_0, v_0_0_1, v_0_1_1, v_0_1_0);
	// f
	mesh.add_face(v_1_0_0, v_1_1_0, v_1_1_1, v_1_0_1);

	// y
	// b
	mesh.add_face(v_0_0_0, v_1_0_0, v_1_0_1, v_0_0_1);
	// f
	mesh.add_face(v_0_1_0, v_0_1_1, v_1_1_1, v_1_1_0);

	// z
	// b
	mesh.add_face(v_0_0_0, v_0_1_0, v_1_1_0, v_1_0_0);
	// f
	mesh.add_face(v_0_0_1, v_1_0_1, v_1_1_1, v_0_1_1);

	return mesh;
}

int main(int argc, char** argv)
{
    int t_x = argc >= 2 ? std::atoi(argv[1]) : 20;
    int t_y = argc >= 2 ? std::atoi(argv[2]) : 20;
    int t_z = argc >= 2 ? std::atoi(argv[3]) : 20;

	auto l = Labyrinth{t_x, t_y, t_z};
	// auto l = Labyrinth{};

	// l.print_paths();

	CGAL::IO::write_OBJ( "mesh.obj", l.to_obj(l) );
	std::cout << "to obj" << std::endl;
	// CGAL::IO::write_OBJ( "mesh.obj", Labyrinth::test_box(l) );
	// CGAL::IO::write_OBJ( "box.obj", test_box() );

    return 0;
}
