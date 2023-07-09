#ifndef CIE_GEO_BINARY_OPERATOR_HPP
#define CIE_GEO_BINARY_OPERATOR_HPP

// --- Internal Includes ---
#include "packages/operators/inc/UnaryOperator.hpp"


namespace cie::geo {


template < Size Dimension,
           concepts::CopyConstructible ValueType = Bool,
           concepts::Numeric CoordinateType = Double >
class BinaryOperator : public UnaryOperator<Dimension,ValueType,CoordinateType>
{
public:
    BinaryOperator() = default;
    BinaryOperator( typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_lhs,
                    typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_rhs );
    BinaryOperator( const BinaryOperator<Dimension,ValueType,CoordinateType>& r_rhs ) = default;
    BinaryOperator<Dimension,ValueType,CoordinateType>& operator=( const BinaryOperator<Dimension,ValueType,CoordinateType>& r_rhs ) = default;

    void bindLhs( typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr p_lhs );

    template <class ObjectType, class ...Args>
    void emplaceLhs( Args&&... args )
        requires concepts::DerivedFrom<ObjectType,Object<Dimension,ValueType,CoordinateType>>;

    typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr& lhs();
    const typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr& lhs() const;

    virtual ValueType at(const typename BinaryOperator<Dimension, ValueType, CoordinateType>::Point& r_point) const override = 0;

protected:
    typename BinaryOperator<Dimension,ValueType,CoordinateType>::operand_ptr _p_lhs;
};


} // namespace cie::geo

#include "packages/operators/impl/BinaryOperator_impl.hpp"

#endif