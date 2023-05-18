#ifndef POS_DEF
#define POS_DEF

#include <string>
#include <sstream>

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

#endif
