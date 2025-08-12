#ifndef CIE_GEO_PARTITIONING_AABBOX_HPP
#define CIE_GEO_PARTITIONING_AABBOX_HPP

// --- Internal Includes ---
#include "packages/primitives/inc/Box.hpp"


namespace cie::geo {


/// @ingroup geo
template <Size Dimension, concepts::Numeric TCoordinate = Double>
class AABBox : public boolean::Box<Dimension,TCoordinate>
{
public:
    using boolean::Box<Dimension,TCoordinate>::Box;

    using boolean::Box<Dimension,TCoordinate>::operator=;

    /// @brief Check whether this box fully encloses the input box.
    /// @param rBox Query box.
    /// @note Closed boundaries.
    Bool contains(const AABBox& rBox) const noexcept;

    /// @brief Check for intersections with the argument box.
    /// @param rBox Query box.
    /// @note Touching does not constitute an intersection.
    Bool intersects(const AABBox& rBox) const noexcept;

    /// @brief Expand the box to include target box.
    /// @param rBox Target box.
    /// @note The target box will not necessarily be "contained".
    void include(const AABBox& rBox) noexcept;
};


} // namespace cie::geo

#include "packages/partitioning/impl/AABBox_impl.hpp"

#endif
