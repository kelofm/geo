#ifndef CIE_GEO_UNARY_OPERATOR_HPP
#define CIE_GEO_UNARY_OPERATOR_HPP

// --- Internal Includes ---
#include "packages/primitives/inc/Object.hpp"


namespace cie::geo {


template < Size Dimension, 
           concepts::CopyConstructible ValueType = Bool,
           concepts::Numeric CoordinateType = Double >
class UnaryOperator : public Object<Dimension,ValueType,CoordinateType>
{
public:
    using operand_type = Object<Dimension,ValueType,CoordinateType>;
    using operand_ptr  = std::shared_ptr<operand_type>;

public:
    UnaryOperator() = default;

    UnaryOperator( operand_ptr p_rhs );

    UnaryOperator( const UnaryOperator<Dimension,ValueType,CoordinateType>& r_rhs ) = default;
    
    UnaryOperator<Dimension,ValueType,CoordinateType>& operator=( const UnaryOperator<Dimension,ValueType,CoordinateType>& r_rhs ) = default;

    void bindRhs( operand_ptr p_rhs );

    template <class ObjectType, class ...Args>
    void emplaceRhs( Args&&... args )
        requires concepts::DerivedFrom<ObjectType, typename UnaryOperator<Dimension, ValueType, CoordinateType>::operand_type>;

    operand_ptr& rhs();
    const operand_ptr& rhs() const;

    virtual ValueType at(const typename UnaryOperator<Dimension, ValueType, CoordinateType>::Point& r_point) const override = 0;

protected:
    operand_ptr _p_rhs;
};


} // namespace cie::geo

#include "packages/operators/impl/UnaryOperator_impl.hpp"

#endif