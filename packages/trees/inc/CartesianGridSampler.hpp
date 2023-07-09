#ifndef CIE_GEO_TREES_CARTESIAN_GRID_SAMPLER_HPP
#define CIE_GEO_TREES_CARTESIAN_GRID_SAMPLER_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/partial_specialization.hpp"
#include "packages/macros/inc/typedefs.hpp"

// --- GEO Includes ---
#include "packages/macros/inc/typedefs.hpp"
#include "packages/trees/inc/PrimitiveSampler.hpp"
#include "packages/primitives/inc/Box.hpp"
#include "packages/primitives/inc/Cube.hpp"
#include "packages/trees/inc/CartesianIndexConverter.hpp"


namespace cie::geo {


/// Interface for cartesian samplers
template <concepts::Primitive TPrimitive>
class AbsCartesianGridSampler : public PrimitiveSampler<TPrimitive>
{
private:
    using IndexConverter = CartesianIndexConverter<TPrimitive::Dimension,Size,Size,tags::Eager>;

public:
    using Resolution = typename IndexConverter::Resolution;

public:
    AbsCartesianGridSampler(const typename IndexConverter::Resolution& r_resolution);

    CIE_DEFINE_CLASS_DEFAULT_MOVES(AbsCartesianGridSampler)

    virtual Size size() const override;

    void setResolution(const Resolution& r_resolution);

protected:
    typename IndexConverter::SpatialIndex getSpatialIndex(Size flatIndex) const;

    const Resolution& getResolution() const noexcept;

private:
    typename IndexConverter::UniquePointer _p_indexConverter;

    Resolution _resolution;
};


/// Invalid default template
CIE_DEFINE_INVALID_CLASS_TEMPLATE_TO_SPECIALIZE(CartesianGridSampler)


/// Cube sampler
template <concepts::Cube TPrimitive>
class CartesianGridSampler<TPrimitive> : public AbsCartesianGridSampler<TPrimitive>
{
public:
    using typename AbsCartesianGridSampler<TPrimitive>::Point;

    using typename AbsCartesianGridSampler<TPrimitive>::Resolution;

public:
    CartesianGridSampler(const Resolution& r_resolution);

    virtual Point getSamplePoint(const TPrimitive& r_primitive,
                                 Size index) const override;
};


/// Box sampler
template <concepts::Box TPrimitive>
class CartesianGridSampler<TPrimitive> : public AbsCartesianGridSampler<TPrimitive>
{
public:
    using typename AbsCartesianGridSampler<TPrimitive>::Point;

    using typename AbsCartesianGridSampler<TPrimitive>::Resolution;

public:
    CartesianGridSampler(const Resolution& r_resolution);

    virtual Point getSamplePoint(const TPrimitive& r_primitive,
                                 Size index) const override;
};


} // namespace cie::geo

#include "packages/trees/impl/CartesianGridSampler_impl.hpp"

#endif