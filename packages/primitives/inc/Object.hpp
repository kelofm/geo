#pragma once

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"
#include "packages/macros/inc/typedefs.hpp"
#include "packages/types/inc/types.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Traits.hpp"

// --- STL Includes ---
#include <memory>
#include <concepts>


namespace cie::geo {

/// Interface for point-queriable objects
template <Size N, concepts::CopyConstructible TValue, concepts::Numeric TCoordinate>
class Object : public Traits<N,TCoordinate>
{
public:
    using typename Traits<N,TCoordinate>::Point;

    using Base = Object<N,TValue,TCoordinate>;

    using value_type = TValue;

    CIE_DEFINE_CLASS_POINTERS(Object)

public:
    virtual ~Object() {}

    virtual TValue at(const typename Object::Point& point) const = 0;
};


// ADL
template <Size Dimension, concepts::CopyConstructible TValue, concepts::Numeric TCoordinate>
using ObjectPtr = std::shared_ptr<Object<Dimension,TValue,TCoordinate>>;


/* --- Convenience Functions --- */

namespace detail {

template <Size Dimension, concepts::Numeric TCoordinate>
typename geo::Traits<Dimension,TCoordinate>::Point makeOrigin();

} // namespace detail


} // namespace cie::geo


namespace cie::concepts {
template <class T, class TValue = bool>
concept SamplableGeometry
= Object<T> && requires (const T& rInstance, typename T::Point location) {
    {rInstance.at(location)} -> std::same_as<TValue>;
};
} // namespace cie::concepts


#include "packages/primitives/impl/Object_impl.hpp"
