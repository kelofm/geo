#ifndef CIE_GEO_PARTITIONING_BOUNDING_BOX_IMPL_HPP
#define CIE_GEO_PARTITIONING_BOUNDING_BOX_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"


namespace cie::geo {


template <concepts::HasBoundingBox TObject>
inline const typename TObject::bounding_box&
boundingBox( TObject& r_object )
{
    CIE_BEGIN_EXCEPTION_TRACING

    return r_object.boundingBox();

    CIE_END_EXCEPTION_TRACING
}



template <concepts::StaticContainer TPoint>
requires std::is_same_v<TPoint,typename GetTraits<TPoint>::Type::Point>
inline const AABBox<GetTraits<TPoint>::Type::Dimension,typename GetTraits<TPoint>::Type::Coordinate>
boundingBox( const TPoint& r_point )
{
    CIE_BEGIN_EXCEPTION_TRACING

    constexpr const Size Dimension = GetTraitsT<TPoint>::Dimension;
    using Coordinate = typename GetTraitsT<TPoint>::Coordinate;

    return AABBox<Dimension,Coordinate>(
        r_point,
        detail::makeOrigin<Dimension,Coordinate>()
    );

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif