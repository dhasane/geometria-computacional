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

// esto es un alias

// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
typedef CGAL::Delaunay_triangulation_2<K>                                    DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;

using TPoint2 = K::Point_2;
using TLine2 = K::Line_2;

int main(int argc, char** argv)
{
    int n = argc > 1 ? std::atoi(argv[1]) : 1000;

    VD vd;

    std::random_device r;
    std::default_random_engine gen(r());
    std::uniform_real_distribution<K::FT> dis(-1, 1);

    for (int a = 0; a < n; ++a) {
        vd.insert(TPoint2(dis(gen), dis(gen)));
    }
    return 0;
}
