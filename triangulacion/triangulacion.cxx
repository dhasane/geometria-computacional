
#include <iostream>
#include <vector>
#include <vector>

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

// template <typename T>
// void triangulacion_monotona(typename std::vector<T>::iterator start,
//                             typename std::vector<T>::iterator end,
//                             typename std::vector<V3<int>>::iterator output
//     )
// {
//     std::vector<V3<int>> triangulos;
//     std::cout << "tests" << std::endl;
//
//     while ( start != end )
//     {
//         std::cout << *start << " ";
//         std::cout << *start++ << " ";
//         std::cout << *end << std::endl;
//         output.push_back({*start, *start++, *end});
//         if ( start != end )
//             triangulos.push_back({*start, *end, *end++});
//     }
// }

int main() {
    // std::vector<_K::Point_2 > points =
    //     {
    //         _K::Point_2( 0, 0 ), // 0
    //         _K::Point_2( 2, 0 ), // 1
    //         _K::Point_2( 2, 2 ), // 2
    //         _K::Point_2( 1, 1 ), // 3
    //         _K::Point_2( 0, 2 )  // 4
    //     };
    // std::vector<_K::Point_2 > points =
    //     {
    //         _K::Point_2( 0, 1 ), // 0
    //             // _K::Point_2( -1, 0 ), // 0
    //         _K::Point_2( 2, 1 ), // 1
    //         _K::Point_2( 2, 3 ), // 2
    //         _K::Point_2( 1, 2 ), // 3
    //         _K::Point_2( 0, 3 )  // 4
    //         };
    std::vector<_K::Point_2 > points =
        {
        _K::Point_2( 1,1 ), // 1
            _K::Point_2( 4,3 ), // 2
            _K::Point_2( 5,5 ), // 3
            _K::Point_2( 3,7 ), // 4
            _K::Point_2( 3,9 ), // 5
            _K::Point_2( 8,8 ), // 6
            _K::Point_2( 9,4 ), // 7
            _K::Point_2( 6,1 ) // 8
            };
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
    std::vector<V3<int>> triangulos;
    for( const auto& poly: partition )
    {
        auto container = poly.container( );

        std::deque<int> d(container.begin(), container.end()-- );

        std::cout << "# ";
        for (auto a: d)
        {
            std::cout << a << " ";
        }
        std::cout << std::endl;

        auto start = container.begin();
        auto end = container.end();

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

    return 0;
}
