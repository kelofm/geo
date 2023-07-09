#ifndef CIE_GEO_TRANSFORM_HPP
#define CIE_GEO_TRANSFORM_HPP

// --- LinAlg Includes ---
#include "packages/spatial/inc/SpatialTransform.hpp"

// --- Internal Includes ---
#include "packages/operators/inc/UnaryOperator.hpp"


namespace cie::geo {


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
requires (Dimension==2 || Dimension==3)
class Transform :
    public UnaryOperator<Dimension,ValueType,CoordinateType>,
    public linalg::SpatialTransform<CoordinateType>
{
public:
    using typename UnaryOperator<Dimension,ValueType,CoordinateType>::Point;
    using typename UnaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr;

    using transform_type = linalg::SpatialTransform<CoordinateType>;

public:
    template <class ...Args>
    Transform( operand_ptr p_operand, Args&&... r_args );
    
    Transform() = default;

    Transform( Transform<Dimension,ValueType,CoordinateType>&& r_rhs ) = default;

    Transform( const Transform<Dimension,ValueType,CoordinateType>& r_rhs ) = default;

    Transform<Dimension,ValueType,CoordinateType>& operator=( const Transform<Dimension,ValueType,CoordinateType>& r_rhs ) = default;

    Transform<Dimension,ValueType,CoordinateType>& set( transform_type&& r_transform );

    virtual ValueType at( const Point& r_point ) const override;
};


} // namespace cie::geo

#include "packages/operators/impl/Transform_impl.hpp"

#endif