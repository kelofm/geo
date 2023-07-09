#ifndef CIE_GEO_INTERSECTION_OPERATOR_HPP
#define CIE_GEO_INTERSECTION_OPERATOR_HPP

// --- Internal Includes ---
#include "packages/operators/inc/BinaryOperator.hpp"

// --- STL Includes ---
#include <memory>


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
class Intersection : public BinaryOperator<Dimension,Bool,CoordinateType>
{
public:
    Intersection() = default;
    Intersection( typename Intersection<Dimension,CoordinateType>::operand_ptr p_lhs,
           typename Intersection<Dimension,CoordinateType>::operand_ptr p_rhs );
    Intersection( const Intersection<Dimension,CoordinateType>& r_rhs ) = default;
    Intersection<Dimension,CoordinateType>& operator=( const Intersection<Dimension,CoordinateType>& r_rhs ) = default;

    virtual Bool at( const typename Intersection<Dimension,CoordinateType>::Point& r_point ) const override;
};


template <Size Dimension, concepts::Numeric CoordinateType>
using IntersectionPtr = std::shared_ptr<Intersection<Dimension,CoordinateType>>;


} // namespace cie::geo

#include "packages/operators/impl/Intersection_impl.hpp"

#endif