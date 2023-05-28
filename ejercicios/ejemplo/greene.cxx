
#include <iostream>
#include <vector>
#include <CGAL/Cartesian.h>
#include <CGAL/enum.h>
#include <random>

// esto es un alias
using TKernel = CGAL::Cartesian<double>;
using TPoint2 = TKernel::Point_2;
using TLine2 = TKernel::Line_2;

int main(int argc, char** argv)
{
    std::vector<TPoint2> mispuntos;

    int n = std::atoi(argv[1]);
    int lx0 = std::atoi(argv[2]);
    int lx1 = std::atoi(argv[3]);
    int ly0 = std::atoi(argv[4]);
    int ly1 = std::atoi(argv[5]);
    std::cout << "hola mundo" << std::endl;

    std::random_device r;

    std::default_random_engine gen(r());
    std::uniform_real_distribution<TKernel::FT> dis(-1, 1);

    TLine2 line( TPoint2(lx0, ly0), TPoint2(lx1, ly1) );

    for (int a = 0 ; a < n ; ++ a )
    {
        mispuntos.push_back( TPoint2(dis(gen),dis(gen)) );
    }

    for (const auto & p: mispuntos)
    {
        if( line.oriented_side(p) == CGAL::ON_POSITIVE_SIDE)
        {

        }
    }

    for (const auto & p: mispuntos)
    {
        std::cout << p << std::endl;
    }
    return (0);
}



void generar_punto_aleatorio()
{

}
