#ifndef CIE_GEO_OVERLOADS_HPP
#define CIE_GEO_OVERLOADS_HPP

// --- Internal Includes ---
#include "packages/operators/inc/Complement.hpp"
#include "packages/operators/inc/Intersection.hpp"
#include "packages/operators/inc/Union.hpp"
#include "packages/operators/inc/Subtraction.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric TCoordinate>
ObjectPtr<Dimension,Bool,TCoordinate> operator*(ObjectPtr<Dimension,Bool,TCoordinate> p_lhs,
                                                ObjectPtr<Dimension,Bool,TCoordinate> p_rhs);


template <Size Dimension, concepts::Numeric TCoordinate>
ObjectPtr<Dimension,Bool,TCoordinate> operator+(ObjectPtr<Dimension,Bool,TCoordinate> p_lhs,
                                                ObjectPtr<Dimension,Bool,TCoordinate> p_rhs);


template <Size Dimension, concepts::Numeric TCoordinate>
ObjectPtr<Dimension,Bool,TCoordinate> operator-(ObjectPtr<Dimension,Bool,TCoordinate> p_lhs,
                                                ObjectPtr<Dimension,Bool,TCoordinate> p_rhs);


} // namespace cie::geo

#include "packages/operators/impl/overloads_impl.hpp"

#endif