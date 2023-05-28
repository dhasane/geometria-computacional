
#include <iostream>
#include <vector>
#include <deque>
#include <CGAL/Cartesian.h>
#include <CGAL/enum.h>
#include <random>

// esto es un alias
using TKernel = CGAL::Cartesian<double>;
using TPoint2 = TKernel::Point_2;
using TLine2 = TKernel::Line_2;

std::vector<TLine2> intersecciones(std::vector<TLine2> segmentos)
{
    organizar lista pares de puntos izq der;
    organizar puntos internos de arriba para abajo;


    lista de puntos de interseccion;

    desde primer punto arriba abajo {
        derecha izq
    }
}

void handleEventPoint(TPoint2 p, std::deque<TPoint2> &Q, T)
{
    U;
    L;
    C;

    if (U.size() + L.size() + C.size() > 1)
    {
    }

    if (U.size() + L.size() == 0)
    {
        findNewEvent(U, L, p, Q);
    }
    else
    {

        findNewEvent(U, L, p, Q);
    }
}

TPoint2 interseccion(TLine2 sl, TLine2 sr)
{
    return true
}

void findNewEvent(TLine2 sl, TLine2 sr, TPoint2 p, std::deque<TPoint2> &Q)
{
    TPoint2 intersec = interseccion(sl, sr);
    if(intersec.y() < p.y() || (intersec.y() > p.y() && !Q.contains(p) ))
        Q.push_back(intersec);
}

int main(int argc, char** argv)
{
    return (0);
}

#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef K::Point_2 Point;
typedef K::Segment_2 Segment;

struct Event {
    double x;
    int type;
    Segment segment;
};

struct CompareEvent {
    bool operator() (const Event& e1, const Event& e2) {
        if (e1.x == e2.x) {
            return e1.type < e2.type;
        }
        return e1.x < e2.x;
    }
};

struct CompareSegment {
    bool operator() (const Segment& s1, const Segment& s2) const {
        if (s1.left() != s2.left()) {
            return s1.left() < s2.left();
        }
        return s1.right() < s2.right();
    }
};

std::set<Segment, CompareSegment> status;
std::priority_queue<Event, std::vector<Event>, CompareEvent> events;

void find_intersections(const std::vector<Segment>& segments) {
    for (const auto& segment : segments) {
        events.push({segment.left().x(), 0, segment});
        events.push({segment.right().x(), 1, segment});
    }
    while (!events.empty()) {
        auto event = events.top();
        events.pop();
        if (event.type == 0) { // left endpoint
            auto [iter, _] = status.emplace(event.segment);
            if (std::next(iter) != status.end() && iter->do_intersect(*std::next(iter))) {
                std::cout << "Intersection detected!" << std::endl;
            }
        } else { // right endpoint
            auto iter = status.find(event.segment);
            if (std::prev(iter) != status.end() && std::next(iter) != status.end()) {
                if (std::prev(iter)->do_intersect(*std::next(iter))) {
                    std::cout << "Intersection detected!" << std::endl;
                }
            }
            status.erase(iter);
        }
    }
}

int main() {
    std::vector<Segment> segments = {Segment(Point(0, 0), Point(1, 1)), Segment(Point(0, 1), Point(1, 0))};
    find_intersections(segments);
    return 0;
}
