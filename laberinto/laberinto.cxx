#include <asm-generic/errno.h>
#include <iostream>
#include <vector>
#include <deque>
#include <random>
#include <tuple>
#include <sstream>
#include <string>
#include <algorithm>

std::random_device r;
std::default_random_engine gen{r()};
std::uniform_real_distribution<> dis{0, 1};

int get_random(int to) {
	std::uniform_int_distribution<> dis{0, to - 1};
	return dis(gen);
}

using pos = std::tuple<int, int, int>;
int get_x(pos p) { return std::get<0>(p); }
int get_y(pos p) { return std::get<1>(p); }
int get_z(pos p) { return std::get<2>(p); }

std::string string_pos(pos p) {
	std::ostringstream s;
	s << "("
	  << get_x(p) << ", "
	  << get_y(p) << ", "
	  << get_z(p) << ")";
	return s.str();
}

class Room {
	bool filled;

public:
	std::vector<pos> connected;
	Room() { filled = false; }

	bool is_filled() { return filled; }

	void fill() {
		filled = true;
	}

	void connect(pos c) {
		connected.push_back(c);
	}
};

#define OUTSIDE {-2, -2, -2}

class Labyrinth {

	Room ***rooms;
	pos start;
	pos end;

	int max_x;
	int max_y;
	int max_z;

	bool complete;

	std::deque<std::pair<pos, pos>> potential_rooms;

public:
	Labyrinth(int size_x, int size_y, int size_z) {
		max_x = size_x;
		max_y = size_y;
		max_z = size_z;
		complete = false;

		// matrix cuadrada
		rooms = new Room **[size_x];
		for (int i = 0; i < size_x; ++i) {
			rooms[i] = new Room *[size_y];
			for (int j = 0; j < size_y; ++j) {
				rooms[i][j] = new Room[size_z];
			}
		}

		// siempre empieza en la capa 0 de z, por lo tanto, termina al llegar a max_z
		pos f{get_random(size_x), get_random(size_y), 0};
		start = f;
		potential_rooms.push_back({f, OUTSIDE});

		build();
	}

	void print_sub_paths(pos p, int depth) {
		for (int a = 0; a < depth; a++) {
			std::cout << "-";
		}
		std::cout << string_pos(p);
		if (p == end) {
			std::cout << " - final " ;
		}
		std::cout << std::endl;

		for (auto a : get_room(p)->connected) {
			print_sub_paths(a, depth + 1);
		}
	}

	void print_paths() {
		std::deque<std::pair<pos, int>> dest;

		std::cout << "start: " << string_pos(start) << std::endl;

		print_sub_paths(start, 0);
	}

	void print_potential_rooms () {
		std::cout << "rooms: ";
		for (auto p : potential_rooms) {
			std::cout << string_pos(p.first);
		}
	}

	void print_rooms (std::vector<pos> rooms) {
		for (auto p : rooms) {
			std::cout << string_pos(p);
		}
	}

	void build() {
		while (!potential_rooms.empty()) {
			pos current;
			pos prev;

			// std::shuffle( std::begin( potential_rooms ) , std::end( potential_rooms ) , gen );

			std::tie(current, prev) = potential_rooms.front();

			potential_rooms.pop_front();

			Room *r = get_room(current);

			if (!r->is_filled()) {
				// std::cout << std::endl;
				// std::cout << std::endl;
				std::cout << "from " << string_pos(prev) << " to " << string_pos(current) << std::endl;
				if (0 <= get_x(prev)) {
					get_room(prev)->connect(current);
				}
				r->fill();

				pos next;
				std::vector<pos> not_selected;

                if (select_random_room(current, next, not_selected)) {
                    potential_rooms.push_front({next, current});

                    // std::cout << "next: " << string_pos(next) << std::endl;
                    // std::cout << "not selected: ";
                    // print_rooms(not_selected);

                    for (pos p: not_selected) {
                        potential_rooms.push_back({p, current});
                    }
                }
                // for (pos p: surrounding(current)) {
                //  potential_rooms.push_back({p, current});
                // }
			}
		}
	}

private:
	Room* get_room(pos p) {
		int x = get_x(p);
		int y = get_y(p);
		int z = get_z(p);
		return &rooms[x][y][z];
	}

	bool fill_room(std::vector<pos> &surr, pos from, pos p) {
		int x = get_x(p);
		int y = get_y(p);
		int z = get_z(p);

		if (0 <= x && x < max_x &&
			0 <= y && y < max_y
			)
		{
			if(0 <= z && z < max_z)
			{
				if (!rooms[x][y][z].is_filled()) {
					surr.push_back(p);
				}
			}
			else if (!complete && z >= max_z) {
				// para definir la puerta de salida
				end = from;
				complete = true;
				return true;
			}
		}
		return false;
	}

	std::vector<pos> surrounding(pos p) {
		std::vector<pos> surr;

		int x = get_x(p);
		int y = get_y(p);
		int z = get_z(p);

		// en caso de encontrar el final, no quiero que tome en cuenta las casillas alrededor
		if (fill_room(surr, p, {x + 1, y, z}) ||
			fill_room(surr, p, {x - 1, y, z}) ||
			fill_room(surr, p, {x, y + 1, z}) ||
			fill_room(surr, p, {x, y - 1, z}) ||
			fill_room(surr, p, {x, y, z + 1}) ||
			fill_room(surr, p, {x, y, z - 1})) {

			surr.clear();
			std::cout << "final encontrado " << string_pos(p) << std::endl;
		}

		return surr;
	}

	bool select_random_room(pos current, pos &next, std::vector<pos>& not_selected) {
		std::vector<pos> surr = surrounding(current);

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

int main(int argc, char** argv)
{
    int t_x = argc >= 2 ? std::atoi(argv[1]) : 20;
    int t_y = argc >= 2 ? std::atoi(argv[2]) : 20;
    int t_z = argc >= 2 ? std::atoi(argv[3]) : 20;

	auto l = Labyrinth{t_x, t_y, t_z};

	l.print_paths();

    return 0;
}
