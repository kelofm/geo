#ifndef CIE_GEO_OBJECT_IMPL_HPP
#define CIE_GEO_OBJECT_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/checks.hpp"
#include "packages/stl_extension/inc/resize.hpp"

// --- STL Includes ---
#include <utility>


namespace cie::geo {

// ---------------------------------------------------------
// ABSTRACT BASE
// ---------------------------------------------------------

template < Size Dimension,
           concepts::CopyConstructible TValue,
           concepts::Numeric TCoordinate >
TValue
Object<Dimension,TValue,TCoordinate>::evaluate(const typename Object<Dimension,TValue,TCoordinate>::Point& r_point) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    return this->at( r_point );

    CIE_END_EXCEPTION_TRACING
}


template <Size N, concepts::CopyConstructible TValue, concepts::Numeric TCoordinate>
template <concepts::Container<TCoordinate> TContainer>
TValue
Object<N,TValue,TCoordinate>::evaluate(const TContainer& point) const
{
    CIE_OUT_OF_RANGE_CHECK( point.size() == N )

    typename Object<N,TValue,TCoordinate>::Point internalPoint;
    std::copy(  point.begin(),
                point.end(),
                internalPoint.begin() );
    return this->at(internalPoint);
}


template <Size N, concepts::CopyConstructible TValue, concepts::Numeric TCoordinate>
template <concepts::Container<TCoordinate> TContainer>
TValue
Object<N,TValue,TCoordinate>::evaluate(TContainer&& point) const
{
    CIE_OUT_OF_RANGE_CHECK(point.size() == N)

    typename Object<N,TValue,TCoordinate>::Point internalPoint;
    std::move(point.begin(),
              point.end(),
              internalPoint.begin());
    return this->at(internalPoint);
}


/* --- Convenience Functions --- */

namespace detail {

template <Size Dimension, concepts::Numeric TCoordinate>
typename geo::Traits<Dimension,TCoordinate>::Point makeOrigin()
{
    typename geo::Traits<Dimension,TCoordinate>::Point origin;
    utils::resize(origin, Dimension);

    for (auto& r_component : origin)
        r_component = 0;

    return origin;
}

} // namespace detail


} // namespace cie::geo

#endif