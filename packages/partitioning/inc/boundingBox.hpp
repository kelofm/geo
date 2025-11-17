#pragma once

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/BoxBoundable.hpp"


namespace cie::geo {


/// @brief Bounding box factory for objects with internal bounding boxes.
template <concepts::HasBoundingBox TObject>
const typename TObject::BoundingBox& boundingBox(TObject& rObject) noexcept;


/// @brief Bounding box factory for points.
template <concepts::StaticContainer TPoint>
requires std::is_same_v<TPoint,typename GetTraits<TPoint>::Type::Point>
const AABBox<GetTraits<TPoint>::Type::Dimension,typename GetTraits<TPoint>::Type::Coordinate>
boundingBox(const TPoint& rPoint) noexcept;


} // namespace cie::geo



/* --- CONCEPT DEFINITIONS --- */

namespace cie::concepts {

template <class T>
concept BoxBoundable
= requires (T instance)
{
    {::cie::geo::boundingBox(instance)};
};

} // namespace cie::concepts


#include "packages/partitioning/impl/boundingBox_impl.hpp"
