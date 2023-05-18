#ifndef ROOM_DEF
#define ROOM_DEF

#include <vector>

#include "pos.cxx"

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

#endif
