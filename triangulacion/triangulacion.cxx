
#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <set>


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
using TVector2 = _K::Vector_2;

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

    int top = 0;
    for (int i = 1 ; i < P.size() ; i++)
    {
        if (comp(P[top], P[i]))
        {
            top = i;
        }
    }

	int it_izq = next(top, -1, P.size());
	int it_der = next(top, 1, P.size());

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

	std::pair<int, bool> *prev = NULL;

    // TODO: encontrar como evitar el ultimo paso
	while (it_izq != it_der)
        // while (positive_distance(it_izq, top, P.size()) > positive_distance(it_der, top, P.size()))
	{

		bool right = comp(P[it_izq], P[it_der]);

		int *i = right ? &it_der : &it_izq;
        std::pair<int, bool> current{*i, right};

        if (current.second != S.front().second) // lado contrario
        {
			while( !S.empty() )
			{
				D.push_back({*i, S.front().first});
				S.pop_front();
			}
			if (prev!=NULL)
				S.push_front(*prev);
			S.push_front(current);
        }
        else // mismo lado
        {
			std::pair<int, bool> *last = NULL;
            while( !S.empty() )
            {
                int de = *i;
                int hasta = S.front().first;

				double area;
				if ( de < hasta ) {
					area = area_green(P.begin() + de, P.begin() + hasta );
				} else {
					area = area_green(P.begin() + hasta, P.begin() + de);
				}

                // se puede usar greene para verificar si se sale
                // positivo es interno, negativo es externo
                if ( 0 < area )
                {
                    D.push_back({*i, S.front().first});
                    last = &S.front();
                    S.pop_front();
                } else {
                    break;
                }
            }
            if (last != NULL)
            {
                S.push_front(*last);
            }
        }

		if( 1 < S.size() )
		{
			S.pop_front();
			while( 1 < S.size() )
			{
				D.push_back({current.first, S.front().first});
				S.pop_front();
			}
		}

		*i = next(*i, ((right)? 1 : -1), P.size());

		prev = &current;
	}
}

struct Relaciones {
    std::map<int, std::set<int>> m;

    Relaciones() {

    }

    int size() {
        return m.size();
    }

    void add(int from, int to) {
        this->one_way_add(from, to);
        this->one_way_add(to, from);
    }

    std::set<int> get(int from) {
        return m[from];
    }

    void print() {
        for (std::map<int, std::set<int>>::iterator it = m.begin(); it != m.end(); ++it) {
            std::cout << it->first << ": ( ";
            for (auto a: it->second) {
                std::cout << a << " ";
            }
            std::cout << ")" << std::endl;
        }
    }

    void remove(int from, int to) {
        this->one_way_remove(from, to);
        this->one_way_remove(to, from);
    }

  private:
    void one_way_remove(int from, int to_remove) {
        // no parece muy eficiente
        for (std::set<int>::iterator iter = m[from].begin(); iter != m[from].end();) {
            if (*iter == to_remove) {
                iter = m[from].erase(iter);
                return;
            } else {
                ++iter;
            }
        }
    }
    void one_way_add(int from, int to) {
        if (m.find(from) != m.end()) {
            m[from].insert(to);
        } else {
            m[from] = {to};
        }
    }
};

std::set<std::set<int>> triangulos(Relaciones r) {
    std::set<std::set<int>> triangulos;

    // bueno, esto es espantosamente ineficiente
    // la idea era ir quitando las conexiones ya usadas, pero eso tambien hace que
    // se pierdan otras conexiones
    // cuando tenga mas tiempo arreglo esto
    for (int a = 0 ; a < r.size(); a++) {
        for (auto b: r.get(a)) {
            for (auto c: r.get(b)) {
                std::set<int> c_c = r.get(c);
                if (c_c.find(a) != c_c.end()) {
                    std::set<int> ss;
                    ss.insert(a);
                    ss.insert(b);
                    ss.insert(c);

                    triangulos.insert(ss);
                    // r.remove(a, b);
                    // r.remove(b, c);
                    // r.remove(c, a);
                }
            }
        }
    }
    return triangulos;
}

double cross_product(TVector2 v1, TVector2 v2)
{
    return v1.x() * v2.y() - v1.y() * v2.x();
}

// TODO: revisar esto, que no parece estar correcto
double dotProduct(TLine2 l1, TLine2 l2) {
    TVector2 v1 = l1.to_vector();
    TVector2 v2 = l2.to_vector();

    double dot_product = v1 * v2;

    double angle_rad = atan2(cross_product(v1, v2), dot_product);
    double angle_deg = angle_rad * 180.0 / M_PI;

    // siempre el menor angulo
    double angle = angle_deg > 180 ? 360 - angle_deg : angle_deg;
    return angle;
}

double evaluar_equilateralidad(TPoint2 p1,TPoint2 p2,TPoint2 p3){
    TLine2 l1{p1, p2};
    TLine2 l2{p2, p3};
    TLine2 l3{p3, p1};

    double a1 = dotProduct(l1, l2);
    double a2 = dotProduct(l2, l3);
    double a3 = dotProduct(l3, l1);

    double dp1 = abs( 60 - a1);
    double dp2 = abs( 60 - a2);
    double dp3 = abs( 60 - a3);

    double total = dp1 + dp2 + dp3;
    return total;
}

double evaluar_equilateralidad_poligono(std::set<std::set<int>> triangulos, std::vector<TPoint2> puntos)
{
    double puntaje_total;
	std::cout << "# equilateralidad" << std::endl;
    for (auto t: triangulos) {
        auto tt = t.begin();
        int v1 = *tt;
        tt++;
        int v2 = *tt;
        tt++;
        int v3 = *tt;
        double puntaje_triangulo = evaluar_equilateralidad(puntos[v1],puntos[v2],puntos[v3]);
        puntaje_total += puntaje_triangulo;

		std::cout << "# ";
        for (auto p : t) {
            std::cout << p << " ";
        }
        std::cout << ": " << puntaje_triangulo;
        std::cout << std::endl;
    }
    return puntaje_total;
}

void load_points(std::string filename, std::vector<_K::Point_2> &points, _T::Polygon_2 & polygon) {
    std::ifstream in_str(filename);
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
    load_points(argv[1], points, polygon);

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

    Relaciones r;
	for (const auto &poly : partition) {
        auto container = poly.container();

		std::cout << "# Border" << std::endl;
		// BORDE
        std::cout << "l";
        auto prev = container.begin();
        for (auto p = container.begin(); p != container.end(); p++) {
            std::cout << " " << *p + 1;

            if (p != container.begin() && p != container.end()--) {
                r.add(*prev, *p);
            }
            prev = p;
        }
        r.add(*prev, *container.begin());
        std::cout << " " << *container.begin() + 1;
        std::cout << std::endl;

		// INTERNO

		std::vector<TPoint2> d;
		std::vector<int> pos;

		std::cout << "# internal" << std::endl;
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
            r.add(pos[a.first], pos[a.second]);
			std::cout << "l " << pos[a.first] + 1  << ' ' << pos[a.second] + 1 << std::endl;
		}
    }

    evaluar_equilateralidad_poligono(triangulos(r), points);

    return 0;
}
