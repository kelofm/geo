#ifndef CIE_GEO_PRIMITIVES_ELLIPSOID_IMPL_HPP
#define CIE_GEO_PRIMITIVES_ELLIPSOID_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"


namespace cie::geo {


template < Size Dimension,
           concepts::Numeric CoordinateType >
Ellipsoid<Dimension,CoordinateType>::Ellipsoid( const typename Ellipsoid<Dimension,CoordinateType>::Point& r_center,
                                                const typename Ellipsoid<Dimension,CoordinateType>::Point& r_radii ) :
    _center( r_center ),
    _radii( r_radii )
{
    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
    bool positiveRadii = true;
    for ( const auto& radius : this->_radii )
        if ( radius < 0 )
        {
            positiveRadii = false;
            break;
        }
    
    CIE_RUNTIME_GEOMETRY_CHECK(
        positiveRadii == true,
        "Ellipsoid radii must be non-negative"
    )
    #endif
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
Ellipsoid<Dimension,CoordinateType>::Ellipsoid() :
    Ellipsoid<Dimension,CoordinateType>( detail::makeOrigin<Dimension,CoordinateType>(),
                                         detail::makeOrigin<Dimension,CoordinateType>() )
{
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline Bool
Ellipsoid<Dimension,CoordinateType>::isDegenerate() const
{
    CIE_BEGIN_EXCEPTION_TRACING

    Bool degenerate = false;

    for ( const auto& r_radius : this->_radii )
        if ( r_radius <= 0 )
        {
            degenerate = true;
            break;
        }

    return degenerate;

    CIE_END_EXCEPTION_TRACING
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline typename Ellipsoid<Dimension,CoordinateType>::Point&
Ellipsoid<Dimension,CoordinateType>::center()
{
    return this->_center;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline const typename Ellipsoid<Dimension,CoordinateType>::Point&
Ellipsoid<Dimension,CoordinateType>::center() const
{
    return this->_center;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline typename Ellipsoid<Dimension,CoordinateType>::Point&
Ellipsoid<Dimension,CoordinateType>::radii()
{
    return this->_radii;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline const typename Ellipsoid<Dimension,CoordinateType>::Point&
Ellipsoid<Dimension,CoordinateType>::radii() const
{
    return this->_radii;
}




namespace boolean {


template < Size Dimension,
           concepts::Numeric CoordinateType >
Ellipsoid<Dimension,CoordinateType>::Ellipsoid( const typename Ellipsoid<Dimension,CoordinateType>::Point& r_center,
                                                const typename Ellipsoid<Dimension,CoordinateType>::Point& r_radii ) :
    cie::geo::Ellipsoid<Dimension,CoordinateType>( r_center, r_radii )
{
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
Ellipsoid<Dimension,CoordinateType>::Ellipsoid() :
    cie::geo::Ellipsoid<Dimension,CoordinateType>()
{
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline Bool
Ellipsoid<Dimension,CoordinateType>::at( const typename Ellipsoid<Dimension,CoordinateType>::Point& r_point ) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    CoordinateType value = 0;
    CoordinateType tmp = 0;

    auto it_point    = r_point.begin();
    auto it_pointEnd = r_point.end();
    auto it_center   = this->_center.begin();
    auto it_radii    = this->_radii.begin();

    for ( ; it_point != it_pointEnd; ++it_point,++it_center,++it_radii )
    {
        tmp = ((*it_point) - (*it_center)) / (*it_radii);
        value += tmp * tmp;
    }

    return value <= 1;

    CIE_END_EXCEPTION_TRACING
}


} // namespace boolean


} // namespace cie::geo


#endif