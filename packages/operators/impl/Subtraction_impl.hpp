#ifndef CIE_GEO_SUBTRACTION_OPERATOR_IMPL_HPP
#define CIE_GEO_SUBTRACTION_OPERATOR_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/checks.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
Subtraction<Dimension,CoordinateType>::Subtraction( typename Subtraction<Dimension,CoordinateType>::operand_ptr p_lhs,
                                                    typename Subtraction<Dimension,CoordinateType>::operand_ptr p_rhs ) :
    BinaryOperator<Dimension,Bool,CoordinateType>( p_lhs, p_rhs )
{
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline Bool
Subtraction<Dimension,CoordinateType>::at( const typename Subtraction<Dimension,CoordinateType>::Point& r_point ) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    CIE_CHECK_POINTER( this->_p_lhs )
    CIE_CHECK_POINTER( this->_p_rhs )

    return this->_p_lhs->at( r_point ) && (!this->_p_rhs->at( r_point ));

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif