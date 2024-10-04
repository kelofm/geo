#ifndef CIE_GEO_TRAITS_HPP
#define CIE_GEO_TRAITS_HPP

/// @defgroup geo Geometry Module

// --- Utility Includes ---
#include "packages/types/inc/types.hpp"
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"
#include "packages/stl_extension/inc/StaticArray.hpp"


// ---------------------------------------------------------
// Traits
// ---------------------------------------------------------
namespace cie::geo {

/// @brief Basic templated traits that every object has in GEO.
/// @ingroup geo
template <Size Dim, concepts::Numeric TCoordinate>
class Traits
{
public:
    constexpr inline static const Size Dimension = Dim;

    using Coordinate = TCoordinate;

    using Point = StaticArray<Coordinate,Dimension>;

    using Domain = StaticArray<std::pair<TCoordinate,TCoordinate>,Dimension>;

    using Resolution = StaticArray<Size,Dimension>;
};


} // namespace cie::geo


// ---------------------------------------------------------
// CONCEPTS
// ---------------------------------------------------------
namespace cie::concepts {

template <class T>
concept Object
= requires ()
{
    typename T::Coordinate;
    typename T::Point;
    T::Dimension;
}; // concept Object

} // namespace cie::concepts


// ---------------------------------------------------------
// TRAIT GENERATORS
// ---------------------------------------------------------
namespace cie::geo {


namespace detail {

template <class T>
struct Traits
{
    using Type = void;
};


template <concepts::Object TObject>
struct Traits<TObject>
{
    using Type = geo::Traits<TObject::Dimension,typename TObject::Coordinate>;
};


template <concepts::StaticContainer TPoint>
struct Traits<TPoint>
{
    using Type = geo::Traits<StaticArrayTraits<TPoint>::size,typename TPoint::value_type>;
};


template <class T>
concept HasTraits
= !std::is_same_v<typename Traits<T>::Type,void>;


} // namespace detail


/// @brief Trait extractor.
/// @ingroup geo
template <detail::HasTraits TObject>
struct GetTraits : public detail::Traits<TObject>
{};


template <detail::HasTraits TObject>
using GetTraitsT = typename GetTraits<TObject>::Type;


} // namespace cie::geo


#endif