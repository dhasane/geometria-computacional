// =========================================================================
// @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
// =========================================================================
#ifndef __CGAL_EXT__Cylinder_function__h__
#define __CGAL_EXT__Cylinder_function__h__

#include <CGAL_EXT/Parametric_function.h>

namespace CGAL_EXT
{
    /**
     */
    template< class _K >
    class Cylinder_function
        : public CGAL_EXT::Parametric_function< _K >
    {
    public:
        using TKernel = _K;
        using Self = Cylinder_function;
        using Superclass = CGAL_EXT::Parametric_function< TKernel >;

        using TReal = typename TKernel::FT;
        using TPoint = typename TKernel::Point_3;
        using TVector = typename TKernel::Vector_3;

    public:
        Cylinder_function()
            : Superclass()
        {
        }
        virtual ~Cylinder_function() = default;

        double radius() const
        {
            return(double(this->m_Radius));
        }
        void set_radius(const double& r)
        {
            this->m_Radius = TReal(r);
        }

        double height() const
        {
            return(double(this->m_Height));
        }
        void set_height(const double& h)
        {
            this->m_Height = TReal(h);
        }

        virtual TPoint evaluate(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double r = double(this->m_Radius);
            double h = double(this->m_Height);

            return(TPoint(r * cu, r * su, h * double(v)));
        }
        virtual TVector derivativeU(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double r = double(this->m_Radius);

            return(TVector(-r * su, r * cu, double(0)));
        }
        virtual TVector derivativeV(const TReal& u, const TReal& v) const override
        {
            double h = double(this->m_Height);

            return(TVector(double(0), double(0), h));
        }

        virtual TVector normal(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));

            return(TVector(cu, su, double(0)));
        }

        // Implementación de las curvaturas del cilindro
        virtual std::pair< TReal, TReal > curvatures(
            const TReal& u, const TReal& v
        ) const override
        {
            TReal K = TReal(0);
            TReal H = TReal(0.5) / this->m_Radius;
            return(std::make_pair(K, H));
        }

    protected:
        TReal m_Radius{ 1 };
        TReal m_Height{ 1 };

    };
} // end namespace

// TODO: #include <CGAL_EXT/Cylinder_function.hxx>

#endif // CGAL_EXT__Cylinder_function__h

// eof - $RCSfile$
