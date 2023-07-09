#ifndef CIE_GEO_PRIMITIVES_CUBE_HPP
#define CIE_GEO_PRIMITIVES_CUBE_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"
#include "packages/primitives/inc/concepts.hpp"

// --- STL Includes ---
#include <tuple>
#include <array>


namespace cie::geo {


/// Cube template
template < Size Dimension,
           concepts::Numeric CoordinateType = Double>
class Cube : public AbsPrimitive<Dimension,CoordinateType>
{
public:
    using primitive_constructor_arguments
        = std::tuple<typename Cube::Point, typename Cube::Coordinate>;

public:
    Cube( const typename Cube<Dimension,CoordinateType>::Point& r_base,
          CoordinateType length );

    template <class ContainerType>
    requires concepts::Container<ContainerType,CoordinateType>
    Cube( const ContainerType& base,
          CoordinateType length );

    Cube();

    Cube( const Cube<Dimension,CoordinateType>& r_rhs ) = default;
    Cube<Dimension,CoordinateType>& operator=( const Cube<Dimension,CoordinateType>& r_rhs ) = default;

    virtual Bool isDegenerate() const override;

    const typename Cube<Dimension,CoordinateType>::Point& base() const;
    const typename Cube<Dimension, CoordinateType>::Coordinate& length() const;
    typename Cube<Dimension, CoordinateType>::Point& base();
    typename Cube<Dimension, CoordinateType>::Coordinate& length();

protected:
    typename Cube<Dimension, CoordinateType>::Point       _base;
    typename Cube<Dimension, CoordinateType>::Coordinate  _length;
};


namespace boolean {

/// Cube with point membership test
template <Size Dimension, concepts::Numeric CoordinateType = Double>
class Cube :
    public cie::geo::Cube<Dimension,CoordinateType>,
    public Object<Dimension,Bool,CoordinateType>
{
public:
    Cube( const typename Cube<Dimension,CoordinateType>::Point& r_base,
          CoordinateType length );

    template <class ContainerType>
    requires concepts::Container<ContainerType,CoordinateType>
    Cube( const ContainerType& base, 
          CoordinateType length );

protected:
    virtual Bool at( const typename Cube<Dimension,CoordinateType>::Point& point ) const override;
};

} // namespace boolean


} // namespace cie::geo



namespace cie::concepts {

template <class T>
concept Cube
= Primitive<T>
  && requires ( T instance, const T constInstance )
{
    { instance.base() }         -> std::same_as<typename T::Point&>;
    { constInstance.base() }    -> std::same_as<const typename T::Point&>;
    { instance.length() }       -> std::same_as<typename T::Coordinate&>;
    { constInstance.length() }  -> std::same_as<const typename T::Coordinate&>;
};

} // namespace cie::concepts



#include "packages/primitives/impl/Cube_impl.hpp"

#endif