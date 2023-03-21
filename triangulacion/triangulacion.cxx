
#include <iostream>
#include <vector>
#include <stack>

#include <CGAL/Cartesian.h>
#include <CGAL/partition_2.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/property_map.h>


// esto es un alias
using _K = CGAL::Cartesian< double >;
using _M = CGAL::Pointer_property_map< _K::Point_2 >::type;
using _T = CGAL::Partition_traits_2< _K, _M >;

using TPoint2 = _K::Point_2;
using TLine2 = _K::Line_2;

bool right_segment(TPoint2 origin, TPoint2 punto)
{
    return origin.x() < punto.x();
}

void triangulacion_greedy(std::deque<int> d)
{
	std::cout << "# ";
	for (auto a: d)
	{
		std::cout << a << " ";
	}
	std::cout << std::endl;

	int p1 = d.front();
	d.pop_front();
	int p2 = d.front();
	d.pop_front();
	int p3 = d.back();
	d.pop_back();

	std::cout << "l " << p1 + 1 << " " << p2 + 1 << " " << p3 + 1 << " " << p1 + 1 << std::endl;

	while ( !d.empty() )
	{
		if (d.size() % 2 == 0)
		{
			p1 = p2;
			p2 = d.front();
			d.pop_front();
		}
		else
		{
			p3 = p2;
			p2 = d.back();
			d.pop_back();
		}
		std::cout << "l " << p1 + 1 << " " << p2 + 1 << " " << p3 + 1 << " " << p1 + 1 << std::endl;
	}
}

//template <typename T>
// double area_green(std::vector<TPoint2>::iterator start, std::vector<TPoint2>::iterator end, bool dir)
double area_green(std::vector<TPoint2>::iterator start, std::vector<TPoint2>::iterator end)
{
    // int move = dir ? 1 : -1;
    int move = 1;
    double area = 0;
    for(std::vector<TPoint2>::iterator i=start ; i != std::next(end, - move) ; i = std::next(i, move))
    {
        double x = i->x();
        double y = i->y();

        double xd = std::next(i, move)->x();
        double yd = std::next(i, move)->y();

		// std::cout << x << " " << y << " " << xd << " " << yd << std::endl;

        area += ( ( x * yd ) - ( y * xd ) );
    }

	// std::cout << ">>" << *start << " " << *(end - 1) << std::endl;
	// std::cout << (end-1)->x() << " " << (end-1)->y() << " " << start->x() << " " << start->y() << std::endl;

	area += ( ( (end-move)->x() * start->y() ) - ( (end-move)->y() * start->x() ) );

    return area / 2;
}

// evitar salirse
int next(int act, int inc, int max)
{
    int ch = (act + inc) % max;
    return ch < 0 ? max - 1 : ch;
}

int positive_distance(int cur, int top, int max)
{
    return cur < top ? (max-top) + cur : cur-top;
}

// no funciona perfecto, pero de momento creo que es suficiente
void triangulacion_rot(std::vector<TPoint2> P, std::deque<std::pair<int, int>> &D)
{
    std::deque<std::pair<int, bool>> S;

	// primero es menor que el segundo
    auto comp = [](TPoint2 a, TPoint2 b)
        {
			if (a.y() == b.y()) {
				return a.x() < b.x(); // TODO: revisar esto
			}
			return a.y() < b.y();
        };

    int top = 0 ;
    for (int i = 1 ; i < P.size() ; i++)
    {
        if (comp(P[top], P[i]))
        {
            top = i;
        }
    }

	int it_izq = next(top, -1, P.size());
	int it_der = next(top, 1, P.size());

    // std::cout << it_izq << " " << top << " " << it_der << std::endl;

    // S.push_front({top, false});
    bool right = comp(P[it_izq], P[it_der]);
    if (right)
    {
        S.push_front({it_der, true});
        it_der = next(it_der, 1, P.size());
    }
    else
    {
        S.push_front({it_izq, false});
        it_izq = next(it_izq, -1, P.size());
    }

    // std::cout << to_string<std::pair<int, bool>>(S.begin(), S.end()) << std::endl;

	// while (it_izq != it_der && it_izq != next(it_der, 1, P.size()))
    // TODO: encontrar como evitar el ultimo paso
	while (it_izq != it_der)
        // while (positive_distance(it_izq, top, P.size()) > positive_distance(it_der, top, P.size()))
	{
		int *i;

        // std::cout << "[[[]]]" ; for (auto s: S) {std::cout << s.first << " ";} std::cout << std::endl;
		bool right = comp(P[it_izq], P[it_der]);
		if (right)
		{
			i = &it_der;
		}
		else
		{
			i = &it_izq;
		}

        // std::cout << ">> " << it_izq << ":[" << P[it_izq] << "] " << it_der << ":[" << P[it_der] << "]" << std::endl;
        // std::cout << ">> " << "i:" << *i << "[" << P[*i] << "] S.top:" << S.front().first << ":[" << P[S.front().first] <<"]" << std::endl;

        std::pair<int, bool> *last = NULL;
        std::pair<int, bool> current{*i, right};

        if (right != S.front().second) // lado contrario
        {
            // std::cout << "contrario: " << std::endl;
            last = &S.front();
			while( !S.empty() )
			{
                // std::cout << "contrario: " << *i << " " << S.front().first << std::endl;
				D.push_back({*i, S.front().first});
				// last = &S.front();
				S.pop_front();
			}
            if (last != NULL)
            {
                S.push_front(*last);
                last = NULL;
            }
			S.push_front(current);
        }
        else // mismo lado
        {
            // std::cout << "mismo: " << std::endl;
            double area = 1;

            last = &S.front();
            while( !S.empty() )
            {
                int de = *i;
                int hasta = S.front().first;

                // std::cout << "de " << de << " hasta " << hasta << std::endl;
                area = area_green(
                    P.begin() + (de < hasta ? de : hasta), // esto se deberia organizar
                    P.begin() + (de < hasta ? hasta : de));

                // std::cout << area << std::endl;

                // se puede usar greene para verificar si se sale
                // positivo es interno, negativo es externo
                if ( 0 < area )
                {
                    // std::cout << "mismo: " << *i << " " << S.front().first << std::endl;
                    D.push_back({*i, S.front().first});
                    // last = &S.front();
                    S.pop_front();
                } else {
                    // std::cout << "saliendo" << std::endl;
                    break;
                }
            }
            if (last != NULL && *last != S.front())
            {
                S.push_front(*last);
                last = NULL;
            }
        }

		if (right)
		{
            *i = next(*i, 1, P.size());
		}
		else
		{
            *i = next(*i, -1, P.size());
		}
        // std::cout << "loop" << std::endl;
	}
    // std::cout << "final [[[]]]" ; for (auto s: S) {std::cout << s.first << " ";} std::cout << std::endl;
}

int main( int argc, char** argv )
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " input.obj" << std::endl;
        return (EXIT_FAILURE);
    } // end if

    // Read file
    std::vector<_K::Point_2> points;
    _T::Polygon_2 polygon;
    std::ifstream in_str(argv[1]);
    std::string line;
    std::getline(in_str, line);
    while (!in_str.eof()) {
        if (line[0] == 'v') {
            std::istringstream v(line.substr(1));
            _K::FT x, y, z;
            v >> x >> y >> z;
            points.push_back(_K::Point_2(x, y));
        } else if (line[0] == 'l') {
            std::istringstream l(line.substr(1));
            while (!l.eof()) {
                unsigned long long i;
                l >> i;
                polygon.push_back(i - 1);
            } // end while
            polygon.container().pop_back();
        } // end if
        std::getline(in_str, line);
    } // end while
    in_str.close();

    _T traits(CGAL::make_property_map(points));

    std::list<_T::Polygon_2> partition;
    CGAL::y_monotone_partition_2(polygon.vertices_begin(),
                                 polygon.vertices_end(),
                                 std::back_inserter(partition), traits);

    std::cout << "# Vertices" << std::endl;
    for (const auto &p : points)
        std::cout << "v " << p << " 0" << std::endl;

    std::cout << std::endl;

    std::cout << "# Lines" << std::endl;

    std::cout << "# Border" << std::endl;

    for (const auto &poly : partition) {
        auto container = poly.container();
        std::cout << "l";
        for (int p : container) {
            std::cout << " " << p + 1;
                }
                std::cout << " " << *container.begin() + 1;
                std::cout << std::endl;
        }

        std::cout << "# internal" << std::endl;
        for (const auto &poly : partition) {
                auto container = poly.container();

                // std::cout << "# ========== " << std::endl;
		std::vector<TPoint2> d;
		std::vector<int> pos;

        std::cout << "# ";
        int i = 0;
		for (int p: container)
		{
			TPoint2 point = points[p];
            std::cout << "(" << i << ":" << p+1 << ":[" << point << "]) " ;
            i++;
            d.push_back(point);
            pos.push_back(p);
        }
        std::cout << std::endl;

		std::deque<std::pair<int, int>> out;
		triangulacion_rot(d, out);

		for (auto a : out)
		{
			std::cout << "l " << pos[a.first] + 1  << ' ' << pos[a.second] + 1 << std::endl;
		}
    }

    return 0;
}
