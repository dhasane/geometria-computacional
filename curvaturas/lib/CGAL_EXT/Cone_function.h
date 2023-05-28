// =========================================================================
// @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
// =========================================================================
#ifndef __CGAL_EXT__Cone_function__h__
#define __CGAL_EXT__Cone_function__h__

#include <CGAL_EXT/Parametric_function.h>

namespace CGAL_EXT
{
    /**
     */
    template< class _K >
    class Cone_function
        : public CGAL_EXT::Parametric_function< _K >
    {
    public:
        using TKernel = _K;
        using Self = Cone_function;
        using Superclass = CGAL_EXT::Parametric_function< TKernel >;

        using TReal = typename TKernel::FT;
        using TPoint = typename TKernel::Point_3;
        using TVector = typename TKernel::Vector_3;

    public:
        Cone_function()
            : Superclass()
        {
        }
        virtual ~Cone_function() = default;

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
            double z = (1 - v) * h;

            return(TPoint(r * cu * v, r * su * v, z));
        }
        virtual TVector derivativeU(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double r = double(this->m_Radius);
            double h = double(this->m_Height);

            return(TVector(-r * su * v, r * cu * v, double(0)));
        }
        virtual TVector derivativeV(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double r = double(this->m_Radius);
            double h = double(this->m_Height);

            return(TVector(r * cu, r * su, -h));
        }

        virtual TVector normal(const TReal& u, const TReal& v) const override
        {
            TVector dU = this->derivativeU(u, v);
            TVector dV = this->derivativeV(u, v);

            TVector normal = CGAL::cross_product(dU, dV);
            normal = normal / std::sqrt(normal.squared_length());
            return normal;
        }

        virtual std::pair< TReal, TReal > curvatures(
            const TReal& u, const TReal& v
        ) const override
        {
            double r = double(this->m_Radius);
            double h = double(this->m_Height);
            double slant_height = std::sqrt(h * h + r * r);

            TReal K1 = TReal(0); 
            TReal K2 = TReal(1) / TReal(slant_height); 

            return std::make_pair(K1, K2);
        }
    protected:
        TReal m_Radius{ 1 };
        TReal m_Height{ 1 };
    };
} // end namespace

// TODO: #include <CGAL_EXT/Cone_function.hxx>

#endif // CGAL_EXT__Cone_function__h

// eof - $RCSfile$
