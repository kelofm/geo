#ifndef CIE_GEO_PRIMITIVES_CONCEPTS_HPP
#define CIE_GEO_PRIMITIVES_CONCEPTS_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"


namespace cie::concepts {


// Helper for the Primitive concept
namespace detail {

template <class T, class _ = void>
struct is_primitive : std::false_type {};

template <class ...Args>
struct is_primitive_helper {};

template <class T>
struct is_primitive
<
    T,
    std::conditional_t
    <
        false,
        is_primitive_helper
        <
            typename T::Coordinate,
            typename T::Point,
            typename T::primitive_constructor_arguments
        >,
        void
    >
> : public std::true_type {};


template <class T>
concept hasIsDegenerate
= requires ( T instance )
{
    { instance.isDegenerate() } -> std::same_as<Bool>;
};

} // namespace detail

/**
 * A Primitive requires the following type aliases to be valid:
 *  - Coordinate
 *  - Point
 *  - primitive_constructor_arguments
*/
template <class T>
concept Primitive
= detail::is_primitive<T>::value
    && detail::hasIsDegenerate<T>;


template <class T>
concept PrimitivePtr
= Pointer<T>
  && Primitive<typename std::pointer_traits<T>::element_type>;


template <class T>
concept PrimitiveContainer
= Container<T> && Primitive<typename T::value_type>;


template <class T>
concept PrimitivePtrContainer
= Container<T> && PrimitivePtr<typename T::value_type>;


} // namespace cie::concepts

#endif