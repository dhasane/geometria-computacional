
#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <set>

#include <numeric>      // std::iota
#include <algorithm>    // std::sort, std::stable_sort

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
    int move = 1;
    double area = 0;
    for(std::vector<TPoint2>::iterator i=start ; i != std::next(end, - move) ; i = std::next(i, move))
    {
        double x = i->x();
        double y = i->y();

        double xd = std::next(i, move)->x();
        double yd = std::next(i, move)->y();

        area += ( ( x * yd ) - ( y * xd ) );
    }

	area += ( ( (end-move)->x() * start->y() ) - ( (end-move)->y() * start->x() ) );

    return area / 2;
}

// https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
template <typename T, typename F>
std::vector<size_t> sort_indexes(const std::vector<T> &v, F&& func, bool reverse) {
	// initialize original index locations
	std::vector<size_t> idx(v.size());

	if (reverse)
		std::reverse(idx.begin(), idx.end());

	iota(idx.begin(), idx.end(), 0);

	// sort indexes based on comparing values in v
	// using std::stable_sort instead of std::sort
	// to avoid unnecessary index re-orderings
	// when v contains elements of equal values
	// stable_sort(idx.begin(), idx.end(),
	// 			[&v, &func](size_t i1, size_t i2) {return func(v[i2], v[i1]);});
	stable_sort(idx.begin(), idx.end(),
				[&v, &func](size_t i1, size_t i2) {return func(v[i2], v[i1]);});

	return idx;
}

void triangulacion(std::vector<TPoint2> &P, std::deque<std::pair<int, int>> &D)
{
	// primero es menor que el segundo
    auto comp = [](TPoint2 a, TPoint2 b)
        {
			if (a.y() == b.y()) {
				return a.x() < b.x();
			}
			return a.y() < b.y();
        };

    auto comp_izq = [](TPoint2 a, TPoint2 b)
        {
			return a.x() < b.x();
        };

    std::deque<std::pair<int, bool>> S;

	auto sorted_indexes = sort_indexes(P, comp, area_green(P.begin(), P.end()) < 0);

	for (auto a : sorted_indexes) {
		std::cout << a << " "; // << "<[" << P[a] << "] ";
	}

	std::cout << std::endl;

	auto i = sorted_indexes.begin();

    TPoint2 top = P[*i];

    S.push_front({*i++, false});
    S.push_front({*i++, comp_izq(top, P[*i])});

	for (; i!= sorted_indexes.end() ; i++)
	{
        std::pair<int, bool> *last = NULL;

		// std::cout << top << " < " << P[*i] << std::endl;
        std::pair<int, bool> current{*i, comp_izq(top, P[*i])};

		// std::cout << "actual: " << current.first << " : " << current.second << std::endl;
		// std::cout << "queue: " << S.front().first << " : " << S.front().second << std::endl;

        if (current.second != S.front().second) // lado contrario
        {
            last = &S.front();
			while( !S.empty() )
			{
				D.push_back({current.first, S.front().first});
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
            last = &S.front();
            while( !S.empty() )
            {
                int de = current.first;
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
                    D.push_back({current.first, S.front().first});
                    // last = &S.front();
                    S.pop_front();
                } else {
                    break;
                }
            }
            if (last != NULL && *last != S.front())
            {
                S.push_front(*last);
                last = NULL;
            }
			S.push_front(current);
        }
	}
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
void triangulacion_rot(std::vector<TPoint2> &P, std::deque<std::pair<int, int>> &D)
{
	if (area_green(P.begin(), P.end()) < 0)
		std::reverse(P.begin(), P.end());

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

        std::pair<int, bool> *last = NULL;
        std::pair<int, bool> current{*i, right};

        if (right != S.front().second) // lado contrario
        {
            last = &S.front();
			while( !S.empty() )
			{
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
            double area = 1;

            last = &S.front();
            while( !S.empty() )
            {
                int de = *i;
                int hasta = S.front().first;

                area = area_green(
                    P.begin() + (de < hasta ? de : hasta), // esto se deberia organizar
                    P.begin() + (de < hasta ? hasta : de));

                // se puede usar greene para verificar si se sale
                // positivo es interno, negativo es externo
                if ( 0 < area )
                {
                    D.push_back({*i, S.front().first});
                    // last = &S.front();
                    S.pop_front();
                } else {
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
    for (auto t: triangulos) {
        auto tt = t.begin();
        int v1 = *tt;
        tt++;
        int v2 = *tt;
        tt++;
        int v3 = *tt;
        double puntaje_triangulo = evaluar_equilateralidad(puntos[v1],puntos[v2],puntos[v3]);
        puntaje_total += puntaje_triangulo;

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

    // std::cout << "# Vertices" << std::endl;
    // for (const auto &p : points)
    //     std::cout << "v " << p << " 0" << std::endl;

    // std::cout << std::endl;

    // std::cout << "# Lines" << std::endl;

    // std::cout << "# Border" << std::endl;

    Relaciones r;


    std::vector<_K::Point_2> points_new_order;
    std::vector<std::pair<int, int>> lines_order;

    for (const auto &poly : partition) {
        auto container = poly.container();
        // std::cout << "l";
        auto prev = container.begin();
        for (auto p = container.begin(); p != container.end(); p++) {
            // std::cout << " " << *p + 1;

            if (p != container.begin() && p != container.end()--) {
                r.add(*prev, *p);
                lines_order.push_back({*prev, *p});
            }
            prev = p;
        }
        r.add(*prev, *container.begin());
		lines_order.push_back({*prev, *container.begin()});
        // std::cout << " " << *container.begin() + 1;
        // std::cout << std::endl;
    }

    for (const auto &poly : partition) {
		// vector solo con los puntos necesarios
		std::vector<TPoint2> d;
		for (int p: poly.container())
		{
			TPoint2 point = points[p];
            d.push_back(point);
        }

		std::deque<std::pair<int, int>> out;
		// triangulacion_rot(d, out);
		triangulacion(d, out);

		for (auto a: d) {
			points_new_order.push_back(a);
		}

		for (auto a : out) {
			lines_order.push_back({a.first, a.second});
		}
    }

	std::cout << "# points " << std::endl;
	for (auto a: points_new_order) {
        std::cout << "v " << a << " 0" << std::endl;
	}

	std::cout << "# lines " << std::endl;
	for (auto a : lines_order)
	{
		std::cout << "l " << a.first + 1  << ' ' << a.second + 1 << std::endl;
	}


    std::cout << std::endl;
    // evaluar_equilateralidad_poligono(triangulos(r), points);

    return 0;
}
