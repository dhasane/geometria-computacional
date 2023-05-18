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

float distancia(Pos p1, Pos p2) {
	Pos dist{p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
	return sqrt( dist.x * dist.x +
				 dist.y * dist.y +
				 dist.z * dist.z);
}

class Labyrinth {

	Room ***rooms;
	Pos start;
	Pos end;

	bool complete;

	//               <actual, desde>
	std::deque<std::pair<Pos, Pos>> potential_rooms;

public:
	Pos max;

	Labyrinth(int size_x, int size_y, int size_z) {
		max = {size_x, size_y, size_z};
		complete = false;

		// matrix cuadrada
		rooms = new Room **[max.x];
		std::vector<Pos> posible_inicio;

		Pos centro {max.x/2,max.y/2,max.z/2};
		for (int x = 0; x < max.x; ++x) {
			rooms[x] = new Room *[max.y];
			for (int y = 0; y < max.y; ++y) {
				rooms[x][y] = new Room[max.z];
				for (int z = 0 ; z < max.z ; z++)
				{
					if (centro.x < distancia({x, y, z}, centro)){
						rooms[x][y][z].fill();
					}
					if (z==0 && !rooms[x][y][z].is_filled()) {
						posible_inicio.push_back({x,y,z});
					}
				}
			}
		}

		// siempre empieza en la capa 0 de z, por lo tanto, termina al llegar a max_z

		// Pos f{get_random(max.x), get_random(max.y), 0};
		Pos f{posible_inicio[get_random(posible_inicio.size())]};
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

		// std::cout << "path: ";
		// print_rooms(path);
		// std::cout << std::endl;
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
