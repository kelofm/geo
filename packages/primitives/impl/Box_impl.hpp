#pragma once

// help the language server
#include "packages/primitives/inc/Box.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/checks.hpp"
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {


/* --- Box --- */

template < Size Dimension,
           concepts::Numeric CoordinateType >
Box<Dimension,CoordinateType>::Box(const Point& r_base, const Point& r_lengths) noexcept
    : _base( r_base ),
      _lengths( r_lengths )
{
    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
    for (const auto& length : r_lengths)
        CIE_DEBUG_CHECK(0 <= length, "Edge lengths of a box must be non-negative (" << length << ")")
    #endif
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
template <class ContainerType1, class ContainerType2>
requires concepts::Container<ContainerType1,CoordinateType>
         && concepts::Container<ContainerType2,CoordinateType>
Box<Dimension,CoordinateType>::Box(const ContainerType1& r_base,
                                   const ContainerType2& r_lengths) noexcept
{
    CIE_OUT_OF_RANGE_CHECK( r_base.size() == Dimension )
    CIE_OUT_OF_RANGE_CHECK( r_lengths.size() == Dimension )

    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
    for (const auto& length : r_lengths)
        CIE_DEBUG_CHECK(0 <= length, "Edge lengths of a box must be non-negative")
    #endif

    std::copy(  r_base.begin(),
                r_base.end(),
                _base.begin() );
    std::copy(  r_lengths.begin(),
                r_lengths.end(),
                _lengths.begin() );
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
Box<Dimension,CoordinateType>::Box() noexcept
    : Box<Dimension,CoordinateType>(detail::makeOrigin<Dimension,CoordinateType>(),
                                    detail::makeOrigin<Dimension,CoordinateType>() )
{
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline Bool
Box<Dimension,CoordinateType>::isDegenerate() const
{
    Bool degenerate = false;
    for (const auto& r_length : _lengths)
        if ( r_length < 1e-16 )
        {
            degenerate = true;
            break;
        }

    return degenerate;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline const typename Box<Dimension,CoordinateType>::Point&
Box<Dimension,CoordinateType>::base() const noexcept
{
    return _base;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline const typename Box<Dimension,CoordinateType>::Point&
Box<Dimension,CoordinateType>::lengths() const noexcept
{
    return _lengths;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline typename Box<Dimension,CoordinateType>::Point&
Box<Dimension,CoordinateType>::base() noexcept
{
    return _base;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline typename Box<Dimension,CoordinateType>::Point&
Box<Dimension,CoordinateType>::lengths() noexcept
{
    return _lengths;
}



/* --- boolean::Box --- */

namespace boolean {


template <  Size Dimension,
            concepts::Numeric CoordinateType   >
template <class ContainerType1, class ContainerType2>
    requires concepts::Container<ContainerType1,CoordinateType>
                && concepts::Container<ContainerType2,CoordinateType>
Box<Dimension,CoordinateType>::Box( const ContainerType1& r_base,
                                    const ContainerType2& r_lengths  ) noexcept
    : cie::geo::Box<Dimension,CoordinateType>(r_base, r_lengths)
{
}


template <Size Dimension, concepts::Numeric CoordinateType>
Bool Box<Dimension,CoordinateType>::at(const typename Box<Dimension,CoordinateType>::Point& rPoint) const
{
    auto itBase     = this->_base.begin();
    auto itLength   = this->_lengths.begin();
    auto itPointEnd = rPoint.end();

    for (auto itPoint=rPoint.begin(); itPoint!=itPointEnd; ++itPoint,++itBase,++itLength) {
        const bool lessThanLowerBound = (*itPoint) < (*itBase);
        const bool lessThanUpperBound = (*itPoint) <= ((*itBase) + (*itLength));
        if (lessThanLowerBound == lessThanUpperBound) {
            return false;
        }
    }

    return true;
}


} // namespace boolean


} // namespace cie::geo
