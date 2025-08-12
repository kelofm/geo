#pragma once

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
BoxBoundable<Dimension,CoordinateType>::BoxBoundable() noexcept
{
    this->geometryChanged();
}


template <Size Dimension, concepts::Numeric CoordinateType>
void BoxBoundable<Dimension,CoordinateType>::geometryChanged() noexcept
{
    this->_recomputeBoundingBox = true;
}


template <Size Dimension, concepts::Numeric CoordinateType>
const typename BoxBoundable<Dimension,CoordinateType>::BoundingBox&
BoxBoundable<Dimension,CoordinateType>::boundingBox() noexcept
{
    if (this->_recomputeBoundingBox) this->computeBoundingBox();
    return this->_boundingBox;
}


template <Size Dimension, concepts::Numeric CoordinateType>
void BoxBoundable<Dimension,CoordinateType>::computeBoundingBox()
{
    this->computeBoundingBoxImpl(this->_boundingBox);
    this->_recomputeBoundingBox = false;
}


} // namespace cie::geo
