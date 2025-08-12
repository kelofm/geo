#pragma once

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/basic_concepts.hpp"

// --- Internal Includes ---
#include "packages/partitioning/inc/AABBox.hpp"


namespace cie::geo {


/// @ingroup geo
template <Size Dimension, concepts::Numeric TCoordinate>
class BoxBoundable : public Traits<Dimension,TCoordinate>
{
public:
    using BoundingBox = AABBox<Dimension,TCoordinate>;

public:
    BoxBoundable() noexcept;

    virtual ~BoxBoundable() = default;

    const BoundingBox& boundingBox() noexcept;

protected:
    /// @brief Set flag indicating that the bounding box should be recomputed.
    void geometryChanged() noexcept;

    void computeBoundingBox();

    virtual void computeBoundingBoxImpl(BoundingBox& rBox) noexcept = 0;

private:
    BoundingBox _boundingBox;

    bool _recomputeBoundingBox;
};


} // namespace cie::geo



/* --- Concepts --- */

namespace cie::concepts {


template <class T>
concept HasBoundingBox
= requires (T instance)
{
    typename T::BoundingBox;
    {instance.boundingBox()};
};


} // namespace cie::concepts



#include "packages/partitioning/impl/BoxBoundable_impl.hpp"
