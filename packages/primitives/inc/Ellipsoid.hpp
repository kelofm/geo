#ifndef CIE_GEO_PRIMITIVES_ELLIPSOID_HPP
#define CIE_GEO_PRIMITIVES_ELLIPSOID_HPP

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"
#include "packages/primitives/inc/Object.hpp"


namespace cie::geo {


/// Ellipsoid template
template < Size Dimension,
           concepts::Numeric CoordinateType = Double >
class Ellipsoid : public AbsPrimitive<Dimension,CoordinateType>
{
public:
    using typename AbsPrimitive<Dimension,CoordinateType>::Point;
public:
    Ellipsoid( const Point& r_center,
               const Point& r_radii );

    Ellipsoid();

    Ellipsoid( const Ellipsoid<Dimension,CoordinateType>& r_rhs ) = default;
    Ellipsoid( Ellipsoid<Dimension,CoordinateType>&& r_rhs ) = default;
    Ellipsoid<Dimension,CoordinateType>& operator=( const Ellipsoid<Dimension,CoordinateType>& r_rhs ) = default;

    virtual Bool isDegenerate() const override;

    Point& center();
    const Point& center() const;

    Point& radii();
    const Point& radii() const;

protected:
    Point  _center;
    Point  _radii;
};




namespace boolean {


/// Ellipsoid with point membership test
template < Size Dimension,
           concepts::Numeric CoordinateType = Double >
class Ellipsoid :
    public cie::geo::Ellipsoid<Dimension,CoordinateType>,
    public Object<Dimension,Bool,CoordinateType>
{
public:
    using typename cie::geo::Ellipsoid<Dimension,CoordinateType>::Point;
public:
    Ellipsoid( const Point& r_center,
               const Point& r_radii );

    Ellipsoid();

    Ellipsoid( const Ellipsoid<Dimension,CoordinateType>& r_rhs ) = default;
    Ellipsoid( Ellipsoid<Dimension,CoordinateType>&& r_rhs ) = default;
    Ellipsoid<Dimension,CoordinateType>& operator=( const Ellipsoid<Dimension,CoordinateType>& r_rhs ) = default;

protected:
    virtual Bool at( const Point& r_point ) const override;
};


} // namespace boolean


} // namespace cie::geo

#include "packages/primitives/impl/Ellipsoid_impl.hpp"

#endif