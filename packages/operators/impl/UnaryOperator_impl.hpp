#ifndef CIE_GEO_UNARY_OPERATOR_IMPL_HPP
#define CIE_GEO_UNARY_OPERATOR_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"


namespace cie::geo {


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
UnaryOperator<Dimension,ValueType,CoordinateType>::UnaryOperator( typename UnaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_rhs ) :
    _p_rhs( p_rhs )
{
}


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
inline void
UnaryOperator<Dimension,ValueType,CoordinateType>::bindRhs( typename UnaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_rhs )
{
    this->_p_rhs = p_rhs;
}


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
template <class ObjectType, class ...Args>
inline void
UnaryOperator<Dimension,ValueType,CoordinateType>::emplaceRhs( Args&&... args )
    requires concepts::DerivedFrom<ObjectType,typename UnaryOperator<Dimension,ValueType,CoordinateType>::operand_type>
{
    CIE_BEGIN_EXCEPTION_TRACING

    this->_p_rhs = std::make_shared<ObjectType>( 
        std::forward<Args>(args)...
    );

    CIE_END_EXCEPTION_TRACING
}


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
inline typename UnaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr&
UnaryOperator<Dimension,ValueType,CoordinateType>::rhs()
{
    return this->_p_rhs;
}


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
inline const typename UnaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr&
UnaryOperator<Dimension,ValueType,CoordinateType>::rhs() const
{
    return this->_p_rhs;
}


} // namespace cie::geo


#endif