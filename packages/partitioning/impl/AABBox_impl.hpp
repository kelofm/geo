#pragma once

// help the language server
#include "packages/partitioning/inc/AABBox.hpp"

// --- Utility Includes ---
#include "packages/maths/inc/OuterProduct.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric TCoordinate>
Bool AABBox<Dimension,TCoordinate>::contains(const AABBox& rBox) const noexcept
{
    // All points inside
    for (Size dim=0; dim<Dimension; ++dim) {
        if (rBox._base[dim] < this->_base[dim]
            ||
            this->_base[dim] + this->_lengths[dim] < rBox._base[dim] + rBox._lengths[dim])
            return false;
    }

    return true;
}


template <Size Dimension, concepts::Numeric TCoordinate>
Bool AABBox<Dimension,TCoordinate>::intersects(const AABBox& rBox) const noexcept
{
    //if (rBox.contains(*this)) return true;

    bool hasCornerOutside = false;
    bool hasCornerInside = false;

    // At least one point outside and one inside
    StaticArray<std::uint8_t,Dimension> samplePointState;
    do {
        typename AABBox::Point corner;
        for (unsigned iDimension=0u; iDimension<Dimension; ++iDimension) {
            // Construct corner.
            corner[iDimension] = rBox.base()[iDimension];
            if (samplePointState[iDimension]) {
                corner[iDimension] += rBox.lengths()[iDimension];
            }
        } // for iDimension in range(Dimension)

        // Check whether the corner is inside or outside the bbox.
        if (this->at(corner)) hasCornerInside = true;
        else hasCornerOutside = true;

        if (hasCornerInside && hasCornerOutside) return true;
    } while (cie::maths::OuterProduct<Dimension>::next(2u, samplePointState.data()));

    return false;
}


template <Size Dimension, concepts::Numeric TCoordinate>
void AABBox<Dimension,TCoordinate>::include(const AABBox& rBox) noexcept
{
    for (Size dim=0; dim<Dimension; ++dim) {
        auto dBase = rBox._base[dim] - this->_base[dim];

        if (dBase < static_cast<TCoordinate>(0)) {
            this->_base[dim]    += dBase;
            this->_lengths[dim] -= dBase;
        }

        auto thisMax = this->_base[dim] + this->_lengths[dim];
        auto boxMax  = rBox._base[dim] + rBox._lengths[dim];

        if (thisMax < boxMax)
            this->_lengths[dim] = boxMax - this->_base[dim];
    } // for dim in range(Dimension)
}


} // namespace cie::geo
