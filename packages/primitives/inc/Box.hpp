#ifndef CIE_GEO_PRIMITIVES_BOX_HPP
#define CIE_GEO_PRIMITIVES_BOX_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"
#include "packages/primitives/inc/concepts.hpp"

// --- STL Includes ---
#include <tuple>
#include <array>


namespace cie::geo {


/**
 * Box template
*/
template < Size Dimension,
           concepts::Numeric CoordinateType = Double >
class Box : public AbsPrimitive<Dimension,CoordinateType>
{
private:
    using Base = AbsPrimitive<Dimension,CoordinateType>;

public:
    using primitive_constructor_arguments
        = std::tuple<typename Box::Point, typename Box::Point>;

    using typename Base::Point;

public:
    Box(const Point& r_base, const Point& r_lengths);

    template <class ContainerType1, class ContainerType2>
    requires concepts::Container<ContainerType1,CoordinateType>
             && concepts::Container<ContainerType2,CoordinateType>
    Box( const ContainerType1& r_base,
         const ContainerType2& r_lengths );

    Box();

    Box( const Box<Dimension,CoordinateType>& r_rhs ) = default;
    Box<Dimension,CoordinateType>& operator=( const Box<Dimension,CoordinateType>& r_rhs ) = default;

    virtual Bool isDegenerate() const override;

    const typename Box<Dimension,CoordinateType>::Point& base() const;
    const typename Box<Dimension,CoordinateType>::Point& lengths() const;
    typename Box<Dimension,CoordinateType>::Point& base();
    typename Box<Dimension,CoordinateType>::Point& lengths();

protected:
    typename Box<Dimension,CoordinateType>::Point _base;
    typename Box<Dimension,CoordinateType>::Point _lengths;
};


namespace boolean {

/**
 * Box with point membership test
*/
template <Size Dimension, concepts::Numeric CoordinateType = Double>
class Box :
    public cie::geo::Box<Dimension,CoordinateType>,
    public Object<Dimension,Bool,CoordinateType>
{
public:
    Box( const typename Box<Dimension,CoordinateType>::Point& r_base,
         const typename Box<Dimension,CoordinateType>::Point& r_lengths );

    template <class ContainerType1, class ContainerType2>
    requires concepts::Container<ContainerType1,CoordinateType>
             && concepts::Container<ContainerType2,CoordinateType>
    Box( const ContainerType1& r_base,
         const ContainerType2& r_lengths );

protected:
    virtual Bool at( const typename Box<Dimension,CoordinateType>::Point& r_point ) const override;
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