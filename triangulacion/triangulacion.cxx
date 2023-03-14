
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


// debe haber una mejor forma de organizar esto
template <typename T>
struct V3 {
    T v1;
    T v2;
    T v3;
};

// int calidad_triangulo(TPoint2 p1, TPoint2 p2, TPoint2 p3)
// {
//
// }

bool right_segment(TPoint2 origin, TPoint2 punto)
{
    return origin.x() < punto.x();
}

struct PointOrder
{
    int posicion;
    bool right;
};
struct Par
{
    int posicionIni;
    int posicionFin;
};


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
double area_green(std::deque<TPoint2>::iterator start, std::deque<TPoint2>::iterator end, int tam)
{

	std::cout << tam << std::endl;
	if (tam == 1)
		return 1;

    double area = 0;
	std::cout << "green" << std::endl;
    for(std::deque<TPoint2>::iterator i=start, j=i+1 ; j != end ; i++, j++)
    {
        double x = i->x();
        double y = i->y();

        double xd = j->x();
		std::cout << xd << std::endl;
        double yd = j->y();

		std::cout << x << " " << y << " " << xd << " " << yd << std::endl;

        area += ( ( x * yd ) - ( y * xd ) ) / 2;
    }
	std::cout << "aaaa" << std::endl;
	area += ( ( (end-1)->x() * start->y() ) - ( (end-1)->y() * start->x() ) ) / 2;

    return area;
}
// typedef vector<punto>::iterator iterador;
// double green(iterador begin, iterador end) {
// 	double area=0:
// 		iterador i = end - 1;
// 	for (iterador j = begin; j != end; i = j++) {
// 		area+= (i->first + j->first)*(p->second - j->second):
// 	}
// 	return area / 2;
// }

void triangulacion(std::deque<TPoint2> P, std::deque<Par> &D)
{
    // std::deque<Par> D;
    std::deque<PointOrder> S;

    std::sort(P.begin(), P.end(), [](TPoint2 a, TPoint2 b)
        {
			if (a.y() == b.y()) {
				return a.x() < b.x();
			}
			return a.y() < b.y();
        });

    TPoint2 origin = P.front();

    S.push_front({0, false});
    S.push_front({1, right_segment(origin, P[1])});

    for (int i=2; i<P.size() ; i++)
    {
        TPoint2 Uj = P[i];
        bool right = right_segment(origin, Uj);
        bool prev_right = S.front().right;

        PointOrder last;

        PointOrder current{i, right};


        if (right != prev_right) // lado contrario
        {
		std::cout << "bla dif" << std::endl;

			while( 0 <= S.size())
			{
				D.push_back({i, S.front().posicion});
				last = S.front();
				S.pop_front();
			}
			S.push_front(last);
			S.push_front(current);
        }
        else // mismo lado
        {
		std::cout << "bla same" << std::endl;

            // se puede usar greene para verificar si se sale
            // positivo es interno, negativo es externo
			while( 0 <= S.size())
			{
				std::deque<TPoint2>::iterator ini = P.begin() + i;
				std::deque<TPoint2>::iterator end = P.begin() + S.front().posicion;
				std::cout << "bla in S" << std::endl;

				double area = area_green(ini, end, i - S.front().posicion) ;

				std::cout << "area: " << area << std::endl;

				if ( 0 <= area )
				{
					D.push_back({i, S.front().posicion});
					last = S.front();
					S.pop_front();
				}
			}
			S.push_front(last);
        }
    }
}

int next(int act, int inc, int max)
{
    int ch = (act + inc) % max;
    return ch < 0 ? max - 1 : ch;
}

void triangulacion_rot(std::deque<TPoint2> P)
{
    std::deque<Par> D;
    std::deque<PointOrder> S;

	// primero es menor que el segundo
    auto comp = [](TPoint2 a, TPoint2 b)
        {
			if (a.y() == b.y()) {
				return a.x() < b.x();
			}
			return a.y() < b.y();
        };

    // ordenamiento

    int p_ant;
    int p_act = -1 ;
    int p_nxt;

    int inc = -1;
    if (comp(P[0], P[1]))
    {
        inc = 1;
    }

    do
    {
        p_ant = next(p_act, -inc, P.size());
        p_act = next(p_act, inc, P.size());
        p_nxt = next(p_act, inc, P.size());

		std::cout << p_ant << ' ' << p_act << ' ' << p_nxt << std::endl;
    } while (comp(P[p_ant], P[p_act]) == comp(P[p_act], P[p_nxt]));

    // final ordenamiento

    std::cout << p_act << std::endl;

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
    for( const auto& poly: partition )
    {
        auto container = poly.container( );

        // std::deque<int> d(container.begin(), container.end()-- );


		std::deque<TPoint2> d;

		for (int p: container)
		{
			TPoint2 point = points[p];
			d.push_back(point);
		}


		std::cout << "# ";
		for (TPoint2 a: d)
		{
			std::cout << '[' << a << "] ";
		}
		std::cout << std::endl;


		// triangulacion_rot(d);
		std::deque<Par> out;
		triangulacion(d, out);

		for (auto a : out)
		{
			std::cout << a.posicionIni << ' ' << a.posicionFin << std::endl;
		}
    }

    return 0;
}
