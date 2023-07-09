#ifndef CIE_GEO_PRIMITIVES_BOX_IMPL_HPP
#define CIE_GEO_PRIMITIVES_BOX_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/checks.hpp"
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {


/* --- Box --- */

template < Size Dimension,
           concepts::Numeric CoordinateType >
Box<Dimension,CoordinateType>::Box(const Point& r_base, const Point& r_lengths)
    : _base( r_base ),
      _lengths( r_lengths )
{
    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
    for (const auto& length : r_lengths)
        CIE_RUNTIME_GEOMETRY_CHECK(0 <= length, "Edge lengths of a box must be non-negative (" << length << ")")
    #endif
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
template <class ContainerType1, class ContainerType2>
requires concepts::Container<ContainerType1,CoordinateType>
         && concepts::Container<ContainerType2,CoordinateType>
Box<Dimension,CoordinateType>::Box( const ContainerType1& r_base,
                                    const ContainerType2& r_lengths  )
{
    CIE_OUT_OF_RANGE_CHECK( r_base.size() == Dimension )
    CIE_OUT_OF_RANGE_CHECK( r_lengths.size() == Dimension )

    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
    for (const auto& length : r_lengths)
        CIE_RUNTIME_GEOMETRY_CHECK(0 <= length, "Edge lengths of a box must be non-negative")
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
Box<Dimension,CoordinateType>::Box() :
    Box<Dimension,CoordinateType>( detail::makeOrigin<Dimension,CoordinateType>(),
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
Box<Dimension,CoordinateType>::base() const
{
    return _base;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline const typename Box<Dimension,CoordinateType>::Point&
Box<Dimension,CoordinateType>::lengths() const
{
    return _lengths;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline typename Box<Dimension,CoordinateType>::Point&
Box<Dimension,CoordinateType>::base()
{
    return _base;
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
inline typename Box<Dimension,CoordinateType>::Point&
Box<Dimension,CoordinateType>::lengths()
{
    return _lengths;
}



/* --- boolean::Box --- */

namespace boolean {


template <Size Dimension, concepts::Numeric CoordinateType>
Box<Dimension,CoordinateType>::Box( const typename Box<Dimension,CoordinateType>::Point& r_base,
                                    const typename Box<Dimension,CoordinateType>::Point& r_lengths ) :
    cie::geo::Box<Dimension,CoordinateType>( r_base, r_lengths )
{
}


template <  Size Dimension,
            concepts::Numeric CoordinateType   >
template <class ContainerType1, class ContainerType2>
    requires concepts::Container<ContainerType1,CoordinateType>
                && concepts::Container<ContainerType2,CoordinateType>
Box<Dimension,CoordinateType>::Box( const ContainerType1& r_base,
                                    const ContainerType2& r_lengths  ) :
    cie::geo::Box<Dimension,CoordinateType>( r_base, r_lengths )
{
}


template < Size Dimension,
           concepts::Numeric CoordinateType >
Bool
Box<Dimension,CoordinateType>::at( const typename Box<Dimension,CoordinateType>::Point& r_point ) const
{
    CIE_OUT_OF_RANGE_CHECK( r_point.size() == Dimension )

    auto it_base        = this->_base.begin();
    auto it_length      = this->_lengths.begin();
    auto it_pointEnd    = r_point.end();

    for (auto it_point=r_point.begin(); it_point!=it_pointEnd; ++it_point,++it_base,++it_length)
        if ( (*it_point<(*it_base)) == (*it_point<(*it_base + (*it_length))) )
            return false;
    return true;
}


} // namespace boolean


} // namespace cie::geo


#endif