#ifndef CIE_GEO_PRIMITIVE_SAMPLER_HPP
#define CIE_GEO_PRIMITIVE_SAMPLER_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/basic_concepts.hpp"

// --- Internal Includes ---
#include "packages/primitives/inc/concepts.hpp"
#include "packages/macros/inc/typedefs.hpp"


namespace cie::geo {


/// Interface for computing sample points on primitives.
template <concepts::Primitive PrimitiveType>
class PrimitiveSampler :
    public Traits<PrimitiveType::Dimension,typename PrimitiveType::Coordinate>
{
public:
    using primitive_type = PrimitiveType;

    CIE_DEFINE_CLASS_POINTERS(PrimitiveSampler)

public:
    virtual ~PrimitiveSampler() {}

    virtual typename PrimitiveType::Point getSamplePoint( const PrimitiveType& r_primitive, Size index ) const = 0;
    virtual Size size() const = 0;
};


template <class PrimitiveType>
using PrimitiveSamplerPtr   = std::shared_ptr<PrimitiveSampler<PrimitiveType>>;


} // namespace cie::geo

#endif