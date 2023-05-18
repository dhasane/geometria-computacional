#ifndef FILTER_DEF
#define FILTER_DEF

#include "pos.cxx"
#include <functional>

struct Filter {
	static std::function<bool(Pos)> circle(Pos max) {
		const Pos centro {max.x/2,max.y/2,max.z/2};
		return [centro](Pos p) {
			return centro.x < centro.distancia(p);
		};
	}
};

#endif
