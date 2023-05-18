#ifndef POS_DEF
#define POS_DEF

#include <math.h>
#include <string>
#include <sstream>

struct Pos {
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

	bool operator!=(Pos p2) {
		return !(*this == p2);
	}

	Pos add_pos(Pos p) {
		return (Pos){x+p.x, y+p.y, z+p.z};
	}

	float distancia(const Pos &p2) const {
		Pos dist{x - p2.x, y - p2.y, z - p2.z};
		return sqrt( dist.x * dist.x +
					 dist.y * dist.y +
					 dist.z * dist.z);
	}
};

#endif
