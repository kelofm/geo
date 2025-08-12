#pragma once

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric TCoordinate>
Bool AABBox<Dimension,TCoordinate>::contains(const AABBox& rBox) const noexcept
{
    // All points inside
    for (Size dim=0; dim<Dimension; ++dim)
        if (rBox._base[dim] < this->_base[dim]
             ||
             this->_base[dim] + this->_lengths[dim] < rBox._base[dim] + rBox._lengths[dim])
            return false;

    return true;
}


template <Size Dimension, concepts::Numeric TCoordinate>
Bool AABBox<Dimension,TCoordinate>::intersects(const AABBox& rBox) const noexcept
{
    bool hasPointOutside = false;

    // At least one point outside and one inside
    for (Size dim=0; dim<Dimension; ++dim) {
        auto thisMax = this->_base[dim] + this->_lengths[dim];
        auto boxMax  = rBox._base[dim] + rBox._lengths[dim];

        if (rBox._base[dim] < this->_base[dim]) {
            if (boxMax <= this->_base[dim])
                return false;
            else if (boxMax < thisMax)
                hasPointOutside = true;
        } else if (rBox._base[dim] < thisMax) {
            if (thisMax < boxMax)
                hasPointOutside = true;
        } else
            return false;
    }

    return hasPointOutside;
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
