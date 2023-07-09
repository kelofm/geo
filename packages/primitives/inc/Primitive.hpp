#ifndef CIE_GEO_PRIMITIVES_PRIMITIVE_HPP
#define CIE_GEO_PRIMITIVES_PRIMITIVE_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/basic_concepts.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/Object.hpp"


namespace cie::geo {


template < Size Dimension,
           concepts::Numeric CoordinateType = Double >
class AbsPrimitive : public Traits<Dimension,CoordinateType>
{
public:
    virtual ~AbsPrimitive() {}

    virtual Bool isDegenerate() const = 0;
};


} // namespace cie::geo

#endif