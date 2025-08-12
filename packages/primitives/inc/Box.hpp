#ifndef CIE_GEO_PRIMITIVES_BOX_HPP
#define CIE_GEO_PRIMITIVES_BOX_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"
#include "packages/primitives/inc/concepts.hpp"

// --- STL Includes ---
#include <tuple>


namespace cie::geo {


/// @brief Box template.
template <Size Dimension, concepts::Numeric TCoordinate = Double>
class Box : public AbsPrimitive<Dimension,TCoordinate>
{
private:
    using Base = AbsPrimitive<Dimension,TCoordinate>;

public:
    using primitive_constructor_arguments
        = std::tuple<typename Box::Point, typename Box::Point>;

    using typename Base::Point;

public:
    Box() noexcept;

    Box(const Point& rBase, const Point& rLengths) noexcept;

    template <class ContainerType1, class ContainerType2>
    requires concepts::Container<ContainerType1,TCoordinate>
             && concepts::Container<ContainerType2,TCoordinate>
    Box(const ContainerType1& rBase,
        const ContainerType2& rLengths) noexcept;

    virtual Bool isDegenerate() const override;

    const typename Box<Dimension,TCoordinate>::Point& base() const noexcept;

    const typename Box<Dimension,TCoordinate>::Point& lengths() const noexcept;

    typename Box<Dimension,TCoordinate>::Point& base() noexcept;

    typename Box<Dimension,TCoordinate>::Point& lengths() noexcept;

protected:
    typename Box<Dimension,TCoordinate>::Point _base;

    typename Box<Dimension,TCoordinate>::Point _lengths;
};


namespace boolean {


/// @brief Box with point membership test
template <Size Dimension, concepts::Numeric TCoordinate = Double>
class Box
    : public ::cie::geo::Box<Dimension,TCoordinate>,
      public Object<Dimension,Bool,TCoordinate>
{
public:
    Box() noexcept = default;

    template <class ContainerType1, class ContainerType2>
    requires concepts::Container<ContainerType1,TCoordinate>
             && concepts::Container<ContainerType2,TCoordinate>
    Box(const ContainerType1& rBase,
        const ContainerType2& rLengths) noexcept;

protected:
    virtual Bool at(const typename Box<Dimension,TCoordinate>::Point& rPoint) const override;
};


} // namespace boolean


} // namespace cie::geo


namespace cie::concepts {

template <class T>
concept Box
= Primitive<T>
  && requires ( T instance, const T constInstance )
{
    { instance.base() }         -> std::same_as<typename T::Point&>;
    { constInstance.base() }    -> std::same_as<const typename T::Point&>;
    { instance.lengths() }      -> std::same_as<typename T::Point&>;
    { constInstance.lengths() } -> std::same_as<const typename T::Point&>;
};

} // namespace cie::concepts


#include "packages/primitives/impl/Box_impl.hpp"

#endif
