#include <iostream>
#include <vector>
#include <deque>
#include <random>

// includes for defining the Voronoi diagram adaptor
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/draw_triangulation_2.h>

// esto es un alias

// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
typedef CGAL::Delaunay_triangulation_2<K>                                    DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;

using Face   = VD::Face;
using Vertex = VD::Vertex;
using Halfedge = VD::Halfedge;

using TPoint2 = K::Point_2;
using TLine2  = K::Line_2;

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

void process_faces(VD::Face_iterator faces_begin, VD::Face_iterator faces_end) {

    std::map<Face*, double> areas;

    auto calculate_face_area = [](Face &a) {
		std::vector<TPoint2> pts;
		Halfedge he = *a.halfedge();

		while(he.next() != a.halfedge())
		{
			// siempre poner el punto anterior
			pts.push_back(he.previous()->target()->point());
			he = *he.next();
		}

		return area_green(pts.begin(), pts.end());
	};

    auto get_area = [&areas, &calculate_face_area](Face &a) {
		if (areas.find(&a) == areas.end()) {
			double area = calculate_face_area(a);
			areas[&a] = area;
		}
		return areas[&a];
	};

    auto face_area_comp = [&get_area](Face a, Face b) {
		return get_area(a) > get_area(b);
	};

    for (auto it = faces_begin; it != faces_end; it++) {
		get_area(*it);
	}

    // std::sort(faces_begin, faces_end, face_area_comp);

	std::cout << "areas" << std::endl;
	for (auto it = areas.begin(); it != areas.end(); it++)
	{
		std::cout << it->first    // string (key)
				  << ':'
				  << it->second   // string's value
				  << std::endl;
	}

}

int main(int argc, char** argv)
{
    int n = argc > 1 ? std::atoi(argv[1]) : 10;

    VD vd;

    std::random_device r;
    std::default_random_engine gen(r());
    std::uniform_real_distribution<K::FT> dis(-1, 1);

    for (int a = 0; a < n; ++a) {
        vd.insert(TPoint2(dis(gen), dis(gen)));
    }

	// CGAL::draw(vd);

	process_faces(vd.faces_begin(), vd.faces_end());

    return 0;
}
