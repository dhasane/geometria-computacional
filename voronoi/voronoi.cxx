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

using K  = CGAL::Exact_predicates_inexact_constructions_kernel;
using DT = CGAL::Delaunay_triangulation_2<K>;
using AT = CGAL::Delaunay_triangulation_adaptation_traits_2<DT>;
using AP = CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT>;
using VD = CGAL::Voronoi_diagram_2<DT,AT,AP>;

using Face     = VD::Face;
using Vertex   = VD::Vertex;
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

double calculate_face_area (Face &a) {
    std::vector<TPoint2> pts;
    Halfedge he = *a.halfedge();

    bool check_area = true;
    while(he.next() != a.halfedge())
    {
        // siempre poner el punto anterior
        if (he.previous()->has_target()) {
            pts.push_back(he.previous()->target()->point());
            he = *he.next();
        } else {
            check_area = false;
            break;
        }
    }
    if (check_area) {
        return area_green(pts.begin(), pts.end());
    }
    return 0;
};

int get_faces_areas(VD::Face_iterator faces_begin, VD::Face_iterator faces_end) {
    int pos_max = 0;
    double max_area = 0;

    int pos = 0;
    for (auto it = faces_begin; it != faces_end; it++) {
        double area = calculate_face_area(*it);
        std::cout << pos << ": " << area << std::endl;
        if (max_area < area) {
            max_area = area;
            pos_max = pos;
        }
        pos++;
    }

    std::cout << "max area: " << max_area << " pos: " << pos << std::endl;
    return pos_max;
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

	int pos_max = get_faces_areas(vd.faces_begin(), vd.faces_end());

    return 0;
}
