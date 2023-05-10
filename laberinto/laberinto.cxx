#include <asm-generic/errno.h>
#include <iostream>
#include <vector>
#include <deque>
#include <random>
#include <tuple>
#include <sstream>
#include <string>

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
	std::vector<pos> connected;

public:
	Room() { filled = false; }

	bool is_filled() { return filled; }

	void fill(pos from) {
		filled = true;
		connect(from);
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

	void print_potential_rooms () {
		std::cout << "rooms: ";
		for (auto p : potential_rooms) {
			std::cout << string_pos(p.first);
		}
	}

	void print_rooms (std::vector<pos> rooms) {
		std::cout << "rooms: ";
		for (auto p : potential_rooms) {
			std::cout << string_pos(p.first);
		}
	}

	void build() {
		while (!potential_rooms.empty()) {
			pos current;
			pos prev;
			std::tie(current, prev) = potential_rooms.front();

			potential_rooms.pop_front();

			Room *r = &rooms[get_x(current)][get_y(current)][get_z(current)];

			if (!r->is_filled()) {
				std::cout << "from " << string_pos(prev) << " to " << string_pos(current) << std::endl;
				r->fill(prev);

				pos next;
				std::vector<pos> not_selected;

				select_random_room(current, next, not_selected);

				potential_rooms.push_front({next, current});

				for (pos p: not_selected) {
					potential_rooms.push_back({p, current});
				}
			}
		}
	}
private:

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
		if (!fill_room(surr, p, {x + 1, y, z}) &&
			!fill_room(surr, p, {x - 1, y, z}) &&
			!fill_room(surr, p, {x, y + 1, z}) &&
			!fill_room(surr, p, {x, y - 1, z}) &&
			!fill_room(surr, p, {x, y, z + 1}) &&
			!fill_room(surr, p, {x, y, z - 1})) {
			return surr;
		}
		return (std::vector<pos>) {};
	}

	void select_random_room(pos current, pos &next, std::vector<pos>& not_selected) {
		std::vector<pos> surr = surrounding(current);

		// no es lo mas eficiente, pero quiero dejar la idea
		int sel = get_random(surr.size());

		for (int a = 0; a < surr.size(); a ++) {
			if (a == sel) {
				next = surr[a];
			}
			else {
				not_selected.push_back(surr[a]);
			}
		}
	}
};

int main(int argc, char** argv)
{
    int t_x = argc >= 2 ? std::atoi(argv[1]) : 20;
    int t_y = argc >= 2 ? std::atoi(argv[2]) : 20;
    int t_z = argc >= 2 ? std::atoi(argv[3]) : 20;

	auto l = Labyrinth{t_x, t_y, t_z};

    return 0;
}
