#ifndef CIE_GEO_SPHERE_HPP
#define CIE_GEO_SPHERE_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType = Double>
class Sphere : public AbsPrimitive<Dimension,CoordinateType>
{
public:
    Sphere( const typename Sphere<Dimension,CoordinateType>::Point& r_center,
            CoordinateType radius );

    template <class ContainerType>
    requires concepts::Container<ContainerType,CoordinateType>
    Sphere( const ContainerType& r_center,
            CoordinateType radius );

    virtual Bool isDegenerate() const override;

    typename Sphere<Dimension,CoordinateType>::Point& center();
    const typename Sphere<Dimension,CoordinateType>::Point& center() const;
    CoordinateType& radius();
    const CoordinateType& radius() const;

protected:
    typename Sphere<Dimension,CoordinateType>::Point _center;
    CoordinateType                                        _radius;
};



/* --- BOOLEAN SPHERE --- */

namespace boolean {


template <Size Dimension, concepts::Numeric CoordinateType>
class Sphere :
    public cie::geo::Sphere<Dimension,CoordinateType>,
    public Object<Dimension,Bool,CoordinateType>
{
public:
    Sphere( const typename Sphere<Dimension,CoordinateType>::Point& r_center,
            CoordinateType radius );

    template <class ContainerType>
    requires concepts::Container<ContainerType,CoordinateType>
    Sphere( const ContainerType& r_center,
            CoordinateType radius );

    virtual Bool at( const typename Sphere<Dimension,CoordinateType>::Point& r_point ) const override;
};


} // namespace boolean

} // namespace cie::geo

#include "packages/primitives/impl/Sphere_impl.hpp"

#endif