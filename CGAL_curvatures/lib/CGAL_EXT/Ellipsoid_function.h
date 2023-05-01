// =========================================================================
// @author Leonardo Florez-Valencia (florez-l@javeriana.edu.co)
// =========================================================================
#ifndef __CGAL_EXT__Ellipsoid_function__h__
#define __CGAL_EXT__Ellipsoid_function__h__

#include <CGAL_EXT/Parametric_function.h>

namespace CGAL_EXT
{
    /**
     */
    template< class _K >
    class Ellipsoid_function
        : public CGAL_EXT::Parametric_function< _K >
    {
    public:
        using TKernel = _K;
        using Self = Ellipsoid_function;
        using Superclass = CGAL_EXT::Parametric_function< TKernel >;

        using TReal = typename TKernel::FT;
        using TPoint = typename TKernel::Point_3;
        using TVector = typename TKernel::Vector_3;

    public:
        Ellipsoid_function()
            : Superclass()
        {
        }
        virtual ~Ellipsoid_function() = default;

        TReal radiusX() const
        {
            return(this->m_RadiusX);
        }
        TReal radiusY() const
        {
            return(this->m_RadiusY);
        }
        TReal radiusZ() const
        {
            return(this->m_RadiusZ);
        }
        void set_radiusX(const TReal& r)
        {
            this->m_RadiusX = r;
        }
        void set_radiusY(const TReal& r)
        {
            this->m_RadiusY = r;
        }
        void set_radiusZ(const TReal& r)
        {
            this->m_RadiusZ = r;
        }

        virtual TPoint evaluate(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double cv = std::cos(double(v));
            double sv = std::sin(double(v));

            return(TPoint(double(this->m_RadiusX) * cu * sv, double(this->m_RadiusY) * su * sv, double(this->m_RadiusZ) * cv));
        }
        virtual TVector derivativeU(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double cv = std::cos(double(v));
            double sv = std::sin(double(v));

            return(TVector(-double(this->m_RadiusX) * su * sv, double(this->m_RadiusY) * cu * sv, double(0)));
        }
        virtual TVector derivativeU2(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double cv = std::cos(double(v));
            double sv = std::sin(double(v));

            return(TVector(-double(this->m_RadiusX) * cu * sv, -double(this->m_RadiusY) * su * sv, double(0)));
        }
        virtual TVector derivativeV(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double cv = std::cos(double(v));
            double sv = std::sin(double(v));

            return(TVector(double(this->m_RadiusX) * cu * cv, double(this->m_RadiusY) * su * cv, -double(this->m_RadiusZ) * sv));
        }
        virtual TVector derivativeV2(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double cv = std::cos(double(v));
            double sv = std::sin(double(v));

            return(TVector(-double(this->m_RadiusX) * cu * sv, -double(this->m_RadiusY) * su * sv, -double(this->m_RadiusZ) * cv));
        }
        virtual TVector derivativeUV(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double cv = std::cos(double(v));
            double sv = std::sin(double(v));

            return(TVector(-double(this->m_RadiusX) * su * cv, double(this->m_RadiusY) * cu * cv, double(0)));
        }

        virtual TVector normal(const TReal& u, const TReal& v) const override
        {
            double cu = std::cos(double(u));
            double su = std::sin(double(u));
            double cv = std::cos(double(v));
            double sv = std::sin(double(v));

            return(TVector(-double(this->m_RadiusX) * cu * sv, -double(this->m_RadiusY) * su * sv, -double(this->m_RadiusZ) * cv));
        }

        virtual std::pair< TReal, TReal > curvatures(
            const TReal& u, const TReal& v
        ) const override
        {
            // Primer Coef fundamental 
            TVector dU = this->derivativeU(u, v);
            TVector dV = this->derivativeV(u, v);
            TReal E = dU * dU;
            TReal F = dU * dV;
            TReal G = dV * dV;

            // Segundo Coef Fundamentals
            TVector dUU = this->derivativeU2(u, v);
            TVector dUV = this->derivativeUV(u, v);
            TVector dVV = this->derivativeV2(u, v);
            TVector N = this->normal(u, v);
            TReal L = dUU * N;
            TReal M = dUV * N;
            TReal N_coef = dVV * N;

            // Curvatura Gaussiana (K) and Curvatura media (H)
            TReal K = (L * N_coef - M * M) / (E * G - F * F);
            TReal H = (E * N_coef - 2 * F * M + G * L) / (2 * (E * G - F * F));

            return std::make_pair(K, H);
        }

    protected:
        TReal m_RadiusX{ 1 };
        TReal m_RadiusY{ 1 };
        TReal m_RadiusZ{ 1 };
    };
} // end namespace

// TODO: #include <CGAL_EXT/Ellipsoid_function.hxx>

#endif // __CGAL_EXT__Ellipsoid_function__h__

// eof - $RCSfile$