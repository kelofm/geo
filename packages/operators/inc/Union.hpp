#ifndef CIE_GEO_UNION_OPERATOR_HPP
#define CIE_GEO_UNION_OPERATOR_HPP

// --- Internal Includes ---
#include "packages/operators/inc/BinaryOperator.hpp"

// --- STL Includes ---
#include <memory>


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
class Union : public BinaryOperator<Dimension,Bool,CoordinateType>
{
public:
    Union() = default;
    Union( typename Union<Dimension,CoordinateType>::operand_ptr p_lhs,
           typename Union<Dimension,CoordinateType>::operand_ptr p_rhs );
    Union( const Union<Dimension,CoordinateType>& r_rhs ) = default;
    Union<Dimension,CoordinateType>& operator=( const Union<Dimension,CoordinateType>& r_rhs ) = default;

    virtual Bool at( const typename Union<Dimension,CoordinateType>::Point& r_point ) const override;
};


template <Size Dimension, concepts::Numeric CoordinateType>
using UnionPtr = std::shared_ptr<Union<Dimension,CoordinateType>>;


} // namespace cie::geo

#include "packages/operators/impl/Union_impl.hpp"

#endif