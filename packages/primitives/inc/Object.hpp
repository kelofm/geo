#ifndef CIE_GEO_OBJECT_HPP
#define CIE_GEO_OBJECT_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"
#include "packages/macros/inc/typedefs.hpp"
#include "packages/types/inc/types.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Traits.hpp"

// --- STL Includes ---
#include <memory>
#include <array>

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

    TValue evaluate(const Point& r_point) const;

    template <concepts::Container<TCoordinate> TContainer>
    TValue evaluate(const TContainer& point) const;

    template <concepts::Container<TCoordinate> TContainer>
    TValue evaluate(TContainer&& point) const;

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

#include "packages/primitives/impl/Object_impl.hpp"

#endif