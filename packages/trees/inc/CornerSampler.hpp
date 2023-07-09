#ifndef CIE_GEO_TREES_CORNER_SAMPLE_HPP
#define CIE_GEO_TREES_CORNER_SAMPLE_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/partial_specialization.hpp"

// --- GEO Includes ---
#include "packages/trees/inc/PrimitiveSampler.hpp"
#include "packages/primitives/inc/Box.hpp"
#include "packages/primitives/inc/Cube.hpp"
#include "packages/trees/inc/CartesianIndexConverter.hpp"


namespace cie::geo {


CIE_DEFINE_INVALID_CLASS_TEMPLATE_TO_SPECIALIZE( CornerSampler )


template <concepts::Cube TPrimitive>
class CornerSampler<TPrimitive> : public PrimitiveSampler<TPrimitive>
{
private:
    using IndexConverter = CartesianIndexConverter<TPrimitive::Dimension,Size,Size,tags::Eager>;

public:
    CornerSampler();

    virtual typename TPrimitive::Point getSamplePoint(const TPrimitive& r_primitive,
                                                         Size index) const override;

    virtual Size size() const override;

protected:
    typename IndexConverter::UniquePointer _p_indexConverter;
};


template <concepts::Box TPrimitive>
class CornerSampler<TPrimitive> : public PrimitiveSampler<TPrimitive>
{
private:
    using IndexConverter = CartesianIndexConverter<TPrimitive::Dimension,Size,Size,tags::Eager>;

public:
    CornerSampler();

    virtual typename TPrimitive::Point getSamplePoint( const TPrimitive& r_primitive,
                                                               Size index ) const override;

    virtual Size size() const override;

protected:
    typename IndexConverter::UniquePointer _p_indexConverter;
};


} // namespace cie::geo

#include "packages/trees/impl/CornerSampler_impl.hpp"

#endif