
#include <iostream>
#include <vector>
#include <CGAL/Cartesian.h>
#include <CGAL/enum.h>
#include <random>

// esto es un alias
using TKernel = CGAL::Cartesian<double>;
using TPoint2 = TKernel::Point_2;
using TLine2 = TKernel::Line_2;

double area_green(std::vector<TPoint2> iterable)
{
    double area = 0;
    int tam = iterable.size();
    for(int i = 0 ; i < tam; i++)
    {
        double x = iterable[i].x();
        double y = iterable[i].y();

        int p = tam != i + 1 ? i + 1 : 0;
        double xd = iterable[p].x();
        double yd = iterable[p].y();

        area += ( ( x * yd ) - ( y * xd ) ) / 2;
    }

    return area;
}

// template <typename T>
// std::vector<T> order(std::vector<T> vect)
// {
//     std::vector<TLine2> nuevo;
//
//     nuevo.push_back(vect[0]);
//     vect.pop(0);
//
//     while (vect.size() > 0)
//     {
//         for (int b = a ; a < vect.size() ; a ++)
//         {
//             if (a.1 == vect[vect.size() - 1].0)
//             {
//                 nuevo.push_back(a);
//                 vect.delete(b);
//                 b --;
//             }
//             else if (a.0 == vect[vect.size() - 1].1)
//             {
//                 nuevo.push_front(a);
//                 vect.delete(b);
//                 b --;
//             }
//         }
//     }
// }

// std::vector<TLine2> casco_convexo_fb(std::vector<TPoint2> iterable)
// {
//     std::vector<TLine2> mispuntos;
//     for ( auto a : iterable )
//     {
//         for ( auto b : iterable )
//         {
//             if (a!=b)
//             {
//                 bool border = true;
//                 TLine2 line( a, b );
//
//                 for (auto c : iterable)
//                 {
//                     if (c!=a && c!=b)
//                     {
//                         border &= (line.oriented_side(c) == CGAL::ON_POSITIVE_SIDE);
//                     }
//
//                     if (!border)
//                     {
//                         break;
//                     }
//                 }
//
//                 if(border)
//                 {
//                     mispuntos.push_back(line);
//                 }
//             }
//         }
//     }
//
//     return mispuntos;
// }

std::vector<TLine2> casco_convexo_inc(std::vector<TPoint2> iterable)
{
    std::vector<TPoint2> mispuntos;





    return mispuntos;
}

int main(int argc, char** argv)
{
    std::vector<TPoint2> mispuntos;

    int n = std::atoi(argv[1]);
    // int lx0 = std::atoi(argv[2]);
    // int lx1 = std::atoi(argv[3]);
    // int ly0 = std::atoi(argv[4]);
    // int ly1 = std::atoi(argv[5]);

    std::random_device r;

    std::default_random_engine gen(r());
    std::uniform_real_distribution<TKernel::FT> dis(-1, 1);

    // TLine2 line( TPoint2(lx0, ly0), TPoint2(lx1, ly1) );

    // for (const auto & p: mispuntos)
    // {
    //     if( line.oriented_side(p) == CGAL::ON_POSITIVE_SIDE)
    //     {

    //     }
    // }

    // for (int a = 0 ; a < n ; ++ a )
    // {
    // mispuntos.push_back( TPoint2(dis(gen),dis(gen)) );
    // }

    mispuntos.push_back(TPoint2(0,0));
    mispuntos.push_back(TPoint2(50,0));
    mispuntos.push_back(TPoint2(50,50));
    mispuntos.push_back(TPoint2(0,50));

    std::cout << area_green(mispuntos) << std::endl;

    // for (const auto & p: mispuntos)
    // {
    //     std::cout << p << std::endl;
    // }
    return (0);
}
