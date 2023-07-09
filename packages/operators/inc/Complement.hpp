#ifndef CIE_GEO_COMPLEMENT_OPERATOR_HPP
#define CIE_GEO_COMPLEMENT_OPERATOR_HPP

// --- Internal Includes ---
#include "packages/operators/inc/UnaryOperator.hpp"

// --- STL Includes ---
#include <memory>


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
class Complement : public UnaryOperator<Dimension,Bool,CoordinateType>
{
public:
    Complement() = default;
    Complement( typename Complement<Dimension,CoordinateType>::operand_ptr p_rhs );
    Complement( const Complement<Dimension,CoordinateType>& r_rhs ) = default;
    Complement<Dimension,CoordinateType>& operator=( const Complement<Dimension,CoordinateType>& r_rhs ) = default;

    virtual Bool at( const typename Complement<Dimension,CoordinateType>::Point& r_point ) const override;
};


template <Size Dimension, concepts::Numeric CoordinateType>
using ComplementPtr = std::shared_ptr<Complement<Dimension,CoordinateType>>;


} // namespace cie::geo

#include "packages/operators/impl/Complement_impl.hpp"

#endif