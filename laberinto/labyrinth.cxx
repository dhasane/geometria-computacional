#ifndef LABYRINTH_DEF
#define LABYRINTH_DEF

#include <deque>
#include <vector>
#include <random>
#include <tuple>

// CGAL
#include <CGAL/Surface_mesh.h>
#include <CGAL/Cartesian.h>

#include "pos.cxx"
#include "room.cxx"

using TKernel = CGAL::Cartesian< double >;
using TPoint = TKernel::Point_3;
using TMesh = CGAL::Surface_mesh< TPoint >;
using TVertex = TMesh::vertex_index;

std::random_device r;
std::default_random_engine gen{r()};
std::uniform_real_distribution<> dis{0, 1};

#define OUTSIDE {-2, -2, -2}

int get_random(int to) {
	std::uniform_int_distribution<> dis{0, to - 1};
	return dis(gen);
}

class Labyrinth {

	Room ***rooms;

	bool complete;

	//               <actual, desde>
	std::deque<std::pair<Pos, Pos>> potential_rooms;

public:
	Pos start;
	Pos end;
	Pos max;

	Labyrinth(int size_x, int size_y, int size_z, std::function<bool(Pos)> filter = nullptr) {
		max = {size_x, size_y, size_z};
		complete = false;

		// matrix cuadrada
		rooms = new Room **[max.x];
		std::vector<Pos> posible_inicio;
		std::vector<Pos> posible_final;

		for (int x = 0; x < max.x; ++x) {
			rooms[x] = new Room *[max.y];
			for (int y = 0; y < max.y; ++y) {
				rooms[x][y] = new Room[max.z];
				for (int z = 0 ; z < max.z ; z++)
				{
					if (filter != nullptr && filter({x, y, z})) {
						rooms[x][y][z].fill();
					}
					if (z==0 && !rooms[x][y][z].is_filled()) {
						posible_inicio.push_back({x,y,z});
					}
					if (z==max.z-1 && !rooms[x][y][z].is_filled()) {
						posible_final.push_back({x,y,z});
					}
				}
			}
		}

		// siempre empieza en la capa 0 de z, por lo tanto, termina al llegar a max_z

		// Pos f{get_random(max.x), get_random(max.y), 0};
		Pos ini{posible_inicio[get_random(posible_inicio.size())]};
		start = ini;
		potential_rooms.push_back({ini, OUTSIDE});

		if (posible_final.empty()) {
			std::cout << "no se encontro salida" << std::endl;
		}

		Pos fin{posible_final[get_random(posible_final.size())]};
		end = fin;

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

		path.push_back(start);

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
				r->from = prev;
				if (0 <= prev.x) {
					get_room(prev)->connect(current);
				}
				r->fill();

				std::vector<Pos> surr = surrounding(current);

				if (!surr.empty()) {
					std::shuffle( std::begin( surr ) , std::end( surr ) , gen );

                    potential_rooms.push_front({surr.front(), current});

					for (int a = 1; a < surr.size(); a++) {
						potential_rooms.push_back({surr[a], current});
					}
				}
			}
		}
	}

	Room* get_room(Pos p) {
		if (!verify_internal(p))
			return nullptr;

		int x = (p.x);
		int y = (p.y);
		int z = (p.z);

		return &rooms[x][y][z];
	}

private:

	bool verify_internal(Pos p) {
		return
			0 <= p.x && p.x < max.x &&
			0 <= p.y && p.y < max.y &&
			0 <= p.z && p.z < max.z;
	}

	void fill_room(std::vector<Pos> &surr, Pos from, Pos p) {
		if (verify_internal(p)) {
			if (!get_room(p)->is_filled()) {
				surr.push_back(p);
			}
		}
	}

	std::vector<Pos> surrounding(Pos p) {
		std::vector<Pos> surr;

		int x = p.x;
		int y = p.y;
		int z = p.z;

		// en caso de encontrar el final, no quiero que tome en cuenta las casillas alrededor
		fill_room(surr, p, {x + 1, y, z});
		fill_room(surr, p, {x - 1, y, z});
		fill_room(surr, p, {x, y + 1, z});
		fill_room(surr, p, {x, y - 1, z});
		fill_room(surr, p, {x, y, z + 1});
		fill_room(surr, p, {x, y, z - 1});

		return surr;
	}
};

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

#endif
