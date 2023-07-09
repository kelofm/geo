#ifndef CIE_GEO_PARTITIONING_AABBOX_IMPL_HPP
#define CIE_GEO_PARTITIONING_AABBOX_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {


template < Size Dimension,
           concepts::Numeric CoordinateType >
AABBox<Dimension,CoordinateType>::AABBox() :
    boolean::Box<Dimension,Double>( detail::makeOrigin<Dimension,CoordinateType>(), detail::makeOrigin<Dimension,CoordinateType>() )
{
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
AABBox<Dimension,CoordinateType>::AABBox( const typename AABBox<Dimension,CoordinateType>::Point& r_base,
                           const typename AABBox<Dimension,CoordinateType>::Point& r_lengths ) :
    boolean::Box<Dimension,Double>( r_base, r_lengths )
{
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline Bool
AABBox<Dimension,CoordinateType>::contains( const AABBox<Dimension,CoordinateType>& r_box ) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    // All points inside
    for ( Size dim=0; dim<Dimension; ++dim )
        if ( r_box._base[dim] < this->_base[dim]
             ||
             this->_base[dim] + this->_lengths[dim] < r_box._base[dim] + r_box._lengths[dim] )
            return false;

    return true;

    CIE_END_EXCEPTION_TRACING
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline Bool
AABBox<Dimension,CoordinateType>::intersects( const AABBox<Dimension,CoordinateType>& r_box ) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    bool hasPointOutside = false;

    // At least one point outside and one inside
    for ( Size dim=0; dim<Dimension; ++dim )
    {
        auto thisMax = this->_base[dim] + this->_lengths[dim];
        auto boxMax  = r_box._base[dim] + r_box._lengths[dim];

        if ( r_box._base[dim] < this->_base[dim] )
        {
            if ( boxMax <= this->_base[dim] )
                return false;
            else if ( boxMax < thisMax )
                hasPointOutside = true;
        }
        else if ( r_box._base[dim] < thisMax )
        {
            if ( thisMax < boxMax )
                hasPointOutside = true;
        }
        else
            return false;
    }

    return hasPointOutside;

    CIE_END_EXCEPTION_TRACING
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline void
AABBox<Dimension,CoordinateType>::include( const AABBox<Dimension,CoordinateType>& r_box )
{
    CIE_BEGIN_EXCEPTION_TRACING

    for ( Size dim=0; dim<Dimension; ++dim )
    {
        auto dBase = r_box._base[dim] - this->_base[dim];

        if ( dBase < 0.0 )
        {
            this->_base[dim]    += dBase;
            this->_lengths[dim] -= dBase;
        }

        auto thisMax = this->_base[dim] + this->_lengths[dim];
        auto boxMax  = r_box._base[dim] + r_box._lengths[dim];

        if ( thisMax < boxMax )
            this->_lengths[dim] = boxMax - this->_base[dim];
    }

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif