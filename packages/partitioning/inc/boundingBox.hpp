#ifndef CIE_GEO_PARTITIONING_BOUNDING_BOX_HPP
#define CIE_GEO_PARTITIONING_BOUNDING_BOX_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AbsBoundableObject.hpp"
#include "packages/primitives/inc/Object.hpp"


namespace cie::geo {


/// Bounding box factory for objects with internal bounding boxes
template <concepts::HasBoundingBox TObject>
const typename TObject::bounding_box& boundingBox(TObject& r_object);


/// Bounding box factory for points
template <concepts::StaticContainer TPoint>
requires std::is_same_v<TPoint,typename GetTraits<TPoint>::Type::Point>
const AABBox<GetTraits<TPoint>::Type::Dimension,typename GetTraits<TPoint>::Type::Coordinate>
boundingBox(const TPoint& r_point);


} // namespace cie::geo



/* --- CONCEPT DEFINITIONS --- */

namespace cie::concepts {

template <class T>
concept BoxBoundable
= requires ( T instance )
{
    { geo::boundingBox( instance ) };
};

} // namespace cie::concepts


#include "packages/partitioning/impl/boundingBox_impl.hpp"

#endif