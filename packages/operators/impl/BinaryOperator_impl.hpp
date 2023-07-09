#ifndef CIE_GEO_BINARY_OPERATOR_IMPL_HPP
#define CIE_GEO_BINARY_OPERATOR_IMPL_HPP


namespace cie::geo {


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
BinaryOperator<Dimension,ValueType,CoordinateType>::BinaryOperator( typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_lhs,
                                                                    typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_rhs ) :
    UnaryOperator<Dimension,ValueType,CoordinateType>( p_rhs ),
    _p_lhs( p_lhs )
{
}


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
inline void
BinaryOperator<Dimension,ValueType,CoordinateType>::bindLhs( typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_lhs )
{
    this->_p_lhs = p_lhs;
}


template < Size Dimension, 
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType   >
template <class ObjectType, class ...Args>
inline void
BinaryOperator<Dimension,ValueType,CoordinateType>::emplaceLhs( Args&&... args )
    requires concepts::DerivedFrom<ObjectType,Object<Dimension,ValueType,CoordinateType>>
{
    this->_p_lhs = std::make_shared<ObjectType>( 
        std::forward<Args>(args)... );
}


template < Size Dimension, 
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
inline typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr&
BinaryOperator<Dimension,ValueType,CoordinateType>::lhs()
{
    return this->_p_lhs;
}


template < Size Dimension, 
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
inline const typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr&
BinaryOperator<Dimension,ValueType,CoordinateType>::lhs() const
{
    return this->_p_lhs;
}


} // namespace cie::geo

#endif