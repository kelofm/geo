#pragma once

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"


namespace cie::geo {


template <concepts::HasBoundingBox TObject>
inline const typename TObject::BoundingBox&
boundingBox(TObject& rObject) noexcept
{
    return rObject.boundingBox();
}



template <concepts::StaticContainer TPoint>
requires std::is_same_v<TPoint,typename GetTraits<TPoint>::Type::Point>
inline const AABBox<GetTraits<TPoint>::Type::Dimension,typename GetTraits<TPoint>::Type::Coordinate>
boundingBox(const TPoint& rPoint) noexcept
{
    CIE_BEGIN_EXCEPTION_TRACING

    constexpr const Size Dimension = GetTraitsT<TPoint>::Dimension;
    using Coordinate = typename GetTraitsT<TPoint>::Coordinate;

    return AABBox<Dimension,Coordinate>(
        rPoint,
        detail::makeOrigin<Dimension,Coordinate>()
    );

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo
