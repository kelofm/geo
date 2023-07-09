#ifndef CIE_GEO_COMPLEMENT_OPERATOR_IMPL_HPP
#define CIE_GEO_COMPLEMENT_OPERATOR_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/checks.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
Complement<Dimension,CoordinateType>::Complement( typename Complement<Dimension,CoordinateType>::operand_ptr p_rhs ) :
    UnaryOperator<Dimension,Bool,CoordinateType>( p_rhs )
{
}


template <Size Dimension, concepts::Numeric CoordinateType>
inline Bool
Complement<Dimension,CoordinateType>::at( const typename Complement<Dimension,CoordinateType>::Point& r_point ) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    CIE_CHECK_POINTER( this->_p_rhs )

    return !this->_p_rhs->at( r_point );

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif