#pragma once

// help the language server
#include "packages/primitives/inc/Box.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/checks.hpp"
#include "packages/macros/inc/exceptions.hpp"
#include "packages/maths/inc/OuterProduct.hpp" // OuterProduct

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {


/* --- Box --- */

template < Size Dimension,
           concepts::Numeric TCoordinate >
Box<Dimension,TCoordinate>::Box(const Point& r_base, const Point& r_lengths) noexcept
    : _base( r_base ),
      _lengths( r_lengths )
{
    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
    for (const auto& length : r_lengths)
        CIE_DEBUG_CHECK(0 <= length, "Edge lengths of a box must be non-negative (" << length << ")")
    #endif
}


template < Size Dimension,
           concepts::Numeric TCoordinate >
template <class ContainerType1, class ContainerType2>
requires concepts::Container<ContainerType1,TCoordinate>
         && concepts::Container<ContainerType2,TCoordinate>
Box<Dimension,TCoordinate>::Box(const ContainerType1& r_base,
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
           concepts::Numeric TCoordinate >
Box<Dimension,TCoordinate>::Box() noexcept
    : Box<Dimension,TCoordinate>(detail::makeOrigin<Dimension,TCoordinate>(),
                                    detail::makeOrigin<Dimension,TCoordinate>() )
{
}


template < Size Dimension,
           concepts::Numeric TCoordinate >
inline Bool
Box<Dimension,TCoordinate>::isDegenerate() const
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
           concepts::Numeric TCoordinate >
inline const typename Box<Dimension,TCoordinate>::Point&
Box<Dimension,TCoordinate>::base() const noexcept
{
    return _base;
}


template < Size Dimension,
           concepts::Numeric TCoordinate >
inline const typename Box<Dimension,TCoordinate>::Point&
Box<Dimension,TCoordinate>::lengths() const noexcept
{
    return _lengths;
}


template < Size Dimension,
           concepts::Numeric TCoordinate >
inline typename Box<Dimension,TCoordinate>::Point&
Box<Dimension,TCoordinate>::base() noexcept
{
    return _base;
}


template < Size Dimension,
           concepts::Numeric TCoordinate >
inline typename Box<Dimension,TCoordinate>::Point&
Box<Dimension,TCoordinate>::lengths() noexcept
{
    return _lengths;
}


template <Size Dimension, concepts::Numeric TCoordinate>
void Box<Dimension,TCoordinate>::makeCorners(std::span<Point,intPow(2,Dimension)> corners) const noexcept
{
    StaticArray<std::uint8_t,Dimension> state;
    std::fill_n(state.data(), Dimension, static_cast<std::uint8_t>(0));
    auto itCorner = corners.data();

    do {
        for (unsigned iDimension=0u; iDimension<Dimension; ++iDimension) {
            (*itCorner)[iDimension] = _base[iDimension];
            if (state[iDimension]) (*itCorner)[iDimension] += _lengths[iDimension];
        } // for iDimension in range(Dimension)

        ++itCorner;
    } while (::cie::maths::OuterProduct<Dimension>::next(2u, state.data()));
}



/* --- boolean::Box --- */

namespace boolean {


template <  Size Dimension,
            concepts::Numeric TCoordinate   >
template <class ContainerType1, class ContainerType2>
    requires concepts::Container<ContainerType1,TCoordinate>
                && concepts::Container<ContainerType2,TCoordinate>
Box<Dimension,TCoordinate>::Box( const ContainerType1& r_base,
                                    const ContainerType2& r_lengths  ) noexcept
    : cie::geo::Box<Dimension,TCoordinate>(r_base, r_lengths)
{
}


template <Size Dimension, concepts::Numeric TCoordinate>
Bool Box<Dimension,TCoordinate>::at(const typename Box<Dimension,TCoordinate>::Point& rPoint) const
{
    return Box::at(rPoint.data(), this->_base.data(), this->_lengths.data());
}


template <Size Dimension, concepts::Numeric TCoordinate>
template <unsigned iDim>
bool Box<Dimension,TCoordinate>::at(Ptr<const TCoordinate> pPointBegin,
                                    Ptr<const TCoordinate> pBaseBegin,
                                    Ptr<const TCoordinate> pLengthBegin) noexcept
{
    if constexpr (iDim != Dimension) {
        const bool lessThanLowerBound = *pPointBegin < *pBaseBegin;
        const bool lessThanUpperBound = *pPointBegin < (*pBaseBegin) + (*pLengthBegin);
        if (lessThanLowerBound == lessThanUpperBound) return false;

        if constexpr (iDim + 1 < Dimension) {
            if (!Box::template at<iDim+1>(++pPointBegin, ++pBaseBegin, ++pLengthBegin)) return false;
        }
    }

    return true;
}


} // namespace boolean


} // namespace cie::geo
