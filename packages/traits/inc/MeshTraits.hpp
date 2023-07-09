#ifndef CIE_MESH_MESH_TRAITS_HPP
#define CIE_MESH_MESH_TRAITS_HPP

// --- GEO Includes ---
#include "packages/primitives/inc/Traits.hpp"

// --- Utility Includes ---
#include "packages/stl_extension/inc/StaticArray.hpp"

// --- STL Includes ---
#include <array>
#include <utility>


// ---------------------------------------------------------
// MeshTraits
// ---------------------------------------------------------
namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
struct MeshTraits : public geo::Traits<Dimension,CoordinateType>
{
    using Domain     = StaticArray<std::pair<CoordinateType,CoordinateType>,Dimension>;
    using Resolution = StaticArray<Size,Dimension>;
};


} // namespace cie::geo


// ---------------------------------------------------------
// CONCEPTS
// ---------------------------------------------------------
namespace cie::concepts {


namespace detail {

template <class T, class _ = void>
struct isMeshObject : std::false_type {};

template <class ...Args>
struct isMeshObjectHelper {};

template <class T>
struct isMeshObject<
    T,
    std::conditional_t<
        false,
        isMeshObjectHelper<
            typename T::Coordinate,
            typename T::Point,
            typename T::Domain,
            typename T::Resolution
        >,
        void
    >
> : public std::true_type {};

} // namespace detail


template <class T>
concept MeshObject
= detail::isMeshObject<T>::value;


} // namespace cie::concepts


#endif