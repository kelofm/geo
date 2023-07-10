#ifndef CIE_GEO_SPHERE_IMPL_HPP
#define CIE_GEO_SPHERE_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/checks.hpp"
#include "packages/stl_extension/inc/resize.hpp"

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
Sphere<Dimension,CoordinateType>::Sphere( const typename Sphere<Dimension,CoordinateType>::Point& r_center,
                                          CoordinateType radius ) :
    _center( r_center ),
    _radius( radius )
{
    CIE_DEBUG_CHECK(
        this->_radius >= 0.0,
        "Sphere radius must be non-negative"
    )
}


template <Size Dimension, concepts::Numeric CoordinateType>
template <class ContainerType>
requires concepts::Container<ContainerType,CoordinateType>
Sphere<Dimension,CoordinateType>::Sphere( const ContainerType& r_center,
                                          CoordinateType radius )
{
    CIE_OUT_OF_RANGE_CHECK(
        r_center.size() == Dimension
    )

    CIE_DEBUG_CHECK(
        radius >= 0.0,
        "Sphere radius must be non-negative"
    )

    this->_radius = radius;
    utils::resize( this->_center, Dimension );

    std::copy( r_center.begin(),
               r_center.end(),
               this->_center.begin() );
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline Bool
Sphere<Dimension,CoordinateType>::isDegenerate() const
{
    return this->_radius == 0;
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline typename Sphere<Dimension,CoordinateType>::Point&
Sphere<Dimension,CoordinateType>::center()
{
    return this->_center;
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline const typename Sphere<Dimension,CoordinateType>::Point&
Sphere<Dimension,CoordinateType>::center() const
{
    return this->_center;
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline CoordinateType&
Sphere<Dimension,CoordinateType>::radius()
{
    return this->_radius;
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline const CoordinateType&
Sphere<Dimension,CoordinateType>::radius() const
{
    return this->_radius;
}



/* --- BOOLEAN SPHERE --- */

namespace boolean {


template <Size Dimension, concepts::Numeric CoordinateType>
Sphere<Dimension,CoordinateType>::Sphere( const typename Sphere<Dimension,CoordinateType>::Point& r_center,
                                          CoordinateType radius ) :
    cie::geo::Sphere<Dimension,CoordinateType>( r_center, radius )
{
}


template <Size Dimension, concepts::Numeric CoordinateType>
template <class ContainerType>
requires concepts::Container<ContainerType,CoordinateType>
Sphere<Dimension,CoordinateType>::Sphere( const ContainerType& r_center,
                                          CoordinateType radius ) :
    cie::geo::Sphere<Dimension,CoordinateType>( r_center, radius )
{
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline Bool
Sphere<Dimension,CoordinateType>::at( const typename Sphere<Dimension,CoordinateType>::Point& r_point ) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    CoordinateType distance = 0;
    CoordinateType tmp = 0;

    for ( Size dim=0; dim<Dimension; ++dim )
    {
        tmp = this->_center[dim] - r_point[dim];
        distance += tmp*tmp;
    }

    return distance <= this->_radius;

    CIE_END_EXCEPTION_TRACING
}


} // namespace boolean


} // namespace cie::geo

#endif