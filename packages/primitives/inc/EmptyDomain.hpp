#ifndef CIE_GEO_EMPTY_DOMAIN_HPP
#define CIE_GEO_EMPTY_DOMAIN_HPP

// --- Internal Includes ---
#include "packages/primitives/inc/Object.hpp"


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
class EmptyDomain : public Object<Dimension,Bool,CoordinateType>
{
public:
    virtual Bool at( const typename EmptyDomain<Dimension,CoordinateType>::Point& r_point ) const override;
};


} // namespace cie::geo

#include "packages/primitives/impl/EmptyDomain_impl.hpp"

#endif