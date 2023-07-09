#ifndef CIE_GEO_SUBTRACTION_OPERATOR_HPP
#define CIE_GEO_SUBTRACTION_OPERATOR_HPP

// --- Internal Includes ---
#include "packages/operators/inc/BinaryOperator.hpp"

// --- STL Includes ---
#include <memory>


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
class Subtraction : public BinaryOperator<Dimension,Bool,CoordinateType>
{
public:
    Subtraction() = default;
    Subtraction( typename Subtraction<Dimension,CoordinateType>::operand_ptr p_lhs,
           typename Subtraction<Dimension,CoordinateType>::operand_ptr p_rhs );
    Subtraction( const Subtraction<Dimension,CoordinateType>& r_rhs ) = default;
    Subtraction<Dimension,CoordinateType>& operator=( const Subtraction<Dimension,CoordinateType>& r_rhs ) = default;

    virtual Bool at( const typename Subtraction<Dimension,CoordinateType>::Point& r_point ) const override;
};


template <Size Dimension, concepts::Numeric CoordinateType>
using SubtractionPtr = std::shared_ptr<Subtraction<Dimension,CoordinateType>>;


} // namespace cie::geo

#include "packages/operators/impl/Subtraction_impl.hpp"

#endif