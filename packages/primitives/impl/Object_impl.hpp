#pragma once

// --- Utility Includes ---
#include "packages/macros/inc/checks.hpp"
#include "packages/stl_extension/inc/resize.hpp"

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {


/* --- Convenience Functions --- */

namespace detail {

template <Size Dimension, concepts::Numeric TCoordinate>
typename geo::Traits<Dimension,TCoordinate>::Point makeOrigin()
{
    typename geo::Traits<Dimension,TCoordinate>::Point origin;
    utils::resize(origin, Dimension);
    std::fill_n(origin.data(), Dimension, static_cast<TCoordinate>(0));
    return origin;
}

} // namespace detail


} // namespace cie::geo
