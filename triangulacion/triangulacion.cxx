
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
double area_green(std::vector<TPoint2>::iterator start, std::vector<TPoint2>::iterator end, bool dir)
{
    int move = dir ? 1 : -1;
    double area = 0;
    for(std::vector<TPoint2>::iterator i=start ; i != std::next(end, - move) ; i = std::next(i, move))
    {
        double x = i->x();
        double y = i->y();

        double xd = std::next(i, move)->x();
        double yd = std::next(i, move)->y();

		// std::cout << x << " " << y << " " << xd << " " << yd << std::endl;

        area += ( ( x * yd ) - ( y * xd ) ) / 2;
    }

	// std::cout << ">>" << *start << " " << *(end - 1) << std::endl;
	// std::cout << (end-1)->x() << " " << (end-1)->y() << " " << start->x() << " " << start->y() << std::endl;

	area += ( ( (end-1)->x() * start->y() ) - ( (end-1)->y() * start->x() ) ) / 2;

    return area;
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

void triangulacion_rot(std::vector<TPoint2> P, std::deque<std::pair<int, int>> &D)
{
    std::deque<std::pair<int, bool>> S;

	// primero es menor que el segundo
    auto comp = [](TPoint2 a, TPoint2 b)
        {
			if (a.y() == b.y()) {
				return a.x() > b.x();
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

    S.push_front({top, false});
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

	// while (it_izq != it_der && it_izq != next(it_der, 1, P.size()))
    // TODO: encontrar como evitar el ultimo paso
	while (it_izq != it_der)
        // while (positive_distance(it_izq, top, P.size()) > positive_distance(it_der, top, P.size()))
	{
		int *i;

		bool right = comp(P[it_izq], P[it_der]);
		if (right)
		{
			i = &it_der;
		}
		else
		{
			i = &it_izq;
		}

        // std::cout << std::endl;
        // std::cout << ">> " << it_izq << ":[" << P[it_izq] << "] " << it_der << ":[" << P[it_der] << "]" << std::endl;
        // std::cout << ">> " << "i:" << *i << "[" << P[*i] << "] S.top:" << S.front().first << ":[" << P[S.front().first] <<"]" << std::endl;

        std::pair<int, bool> *last = NULL;
        std::pair<int, bool> current{*i, right};

        if (right != S.front().second) // lado contrario
        {
            // std::cout <<"contrario: "<< it_izq << ":[" << P[it_izq] << "] " << it_der << ":[" << P[it_der] << "]" << std::endl;
			while( !S.empty() )
			{
				D.push_back({*i, S.front().first});
				last = &S.front();
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
            // std::cout <<"mismo: "<< it_izq << ":[" << P[it_izq] << "] " << it_der << ":[" << P[it_der] << "]" << std::endl;
            double area = 1;

            while( !S.empty() && 0 < area )
            {
                int de = *i;
                int hasta = S.front().first;
                area = area_green(P.begin() + de, P.begin() + hasta, de < hasta );

                std::cout << "de " << de << " hasta " << hasta << " area: " << area << std::endl;

                // se puede usar greene para verificar si se sale
                // positivo es interno, negativo es externo
                if ( 0 < area )
                {
                    D.push_back({*i, S.front().first});
                    last = &S.front();
                    S.pop_front();
                }
            }
            if (last != NULL)
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
	}
}

int main() {
    // std::vector<_K::Point_2 > points =
    //     {
    //         _K::Point_2( 0, 0 ), // 0
    //         _K::Point_2( 2, 0 ), // 1
    //         _K::Point_2( 2, 2 ), // 2
    //         _K::Point_2( 1, 1 ), // 3
    //         _K::Point_2( 0, 2 )  // 4
    //     };
    std::vector<_K::Point_2 > points =
        {
            _K::Point_2( 0, 1 ), // 0
                // _K::Point_2( -1, 0 ), // 0
            _K::Point_2( 2, 1 ), // 1
            _K::Point_2( 2, 3 ), // 2
            _K::Point_2( 1, 2 ), // 3
            _K::Point_2( 0, 3 )  // 4
            };
    // std::vector<_K::Point_2 > points =
    //     {
    //     _K::Point_2( 1,1 ), // 1
    //         _K::Point_2( 4,3 ), // 2
    //         _K::Point_2( 5,5 ), // 3
    //         _K::Point_2( 3,7 ), // 4
    //         _K::Point_2( 3,9 ), // 5
    //         _K::Point_2( 8,8 ), // 6
    //         _K::Point_2( 9,4 ), // 7
    //         _K::Point_2( 6,1 ) // 8
    //         };
    _T traits( CGAL::make_property_map( points ) );
    _T::Polygon_2 polygon;

    for (int i = 0 ; i < points.size(); i++)
    {
        polygon.push_back( i );
    }

    std::list< _T::Polygon_2 > partition;
    CGAL::y_monotone_partition_2(
        polygon.vertices_begin( ), polygon.vertices_end( ),
        std::back_inserter( partition ), traits
        );

    std::cout << "# Vertices" << std::endl;
    for( const auto& p: points )
        std::cout << "v " << p << " 0" << std::endl;

    std::cout << std::endl;

    std::cout << "# Lines" << std::endl;

    std::cout << "# Border" << std::endl;

    for( const auto& poly: partition )
    {
        auto container = poly.container( );
        std::cout << "l";
		for (int p: container)
		{
            std::cout << " " << p + 1;
		}
        std::cout << " " << *container.begin() + 1 ;
        std::cout << std::endl;
    }

    std::cout << "# internal" << std::endl;
    for( const auto& poly: partition )
    {
        auto container = poly.container( );

        // std::cout << "# ========== " << std::endl;
		std::vector<TPoint2> d;
		std::vector<int> pos;

        std::cout << "# ";
		for (int p: container)
		{
			TPoint2 point = points[p];
            std::cout << "(" << p+1 << ":[" << point << "]) " ;
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
