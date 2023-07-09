#ifndef CIE_GEO_TREES_CARTESIAN_GRID_SAMPLER_IMPL_HPP
#define CIE_GEO_TREES_CARTESIAN_GRID_SAMPLER_IMPL_HPP

// --- GEO Includes ---
#include "packages/trees/inc/CartesianGridSampler.hpp"

// --- Utility Includes ---
#include "packages/stl_extension/inc/zip.hpp"
#include "packages/macros/inc/exceptions.hpp"


namespace cie::geo {


template <concepts::Primitive TPrimitive>
AbsCartesianGridSampler<TPrimitive>::AbsCartesianGridSampler(const typename IndexConverter::Resolution& r_resolution)
    : _p_indexConverter(new IndexConverter(r_resolution)),
      _resolution(r_resolution)
{}


template <concepts::Primitive TPrimitive>
inline Size
AbsCartesianGridSampler<TPrimitive>::size() const
{
    return _p_indexConverter->size();
}


template <concepts::Primitive TPrimitive>
inline void
AbsCartesianGridSampler<TPrimitive>::setResolution(const Resolution& r_resolution)
{
    CIE_BEGIN_EXCEPTION_TRACING
    _p_indexConverter->setResolution(r_resolution);
    _resolution = r_resolution;
    CIE_END_EXCEPTION_TRACING
}


template <concepts::Primitive TPrimitive>
inline const typename AbsCartesianGridSampler<TPrimitive>::Resolution&
AbsCartesianGridSampler<TPrimitive>::getResolution() const noexcept
{
    return _resolution;
}


template <concepts::Primitive TPrimitive>
inline typename AbsCartesianGridSampler<TPrimitive>::IndexConverter::SpatialIndex
AbsCartesianGridSampler<TPrimitive>::getSpatialIndex(Size flatIndex) const
{
    CIE_BEGIN_EXCEPTION_TRACING
    return _p_indexConverter->convert(flatIndex);
    CIE_END_EXCEPTION_TRACING
}


// ---------------------------------------------------------
// SPECIALIZED PRIMITIVE SAMPLERS
// ---------------------------------------------------------

/* --- Cube sampler --- */

template <concepts::Cube TPrimitive>
CartesianGridSampler<TPrimitive>::CartesianGridSampler(const Resolution& r_resolution) :
    AbsCartesianGridSampler<TPrimitive>(r_resolution)
{
}


template <concepts::Cube TPrimitive>
inline typename CartesianGridSampler<TPrimitive>::Point
CartesianGridSampler<TPrimitive>::getSamplePoint(const TPrimitive& r_primitive,
                                                 Size index) const
{
    CIE_BEGIN_EXCEPTION_TRACING
    CIE_PROFILE_SCOPE

    using CoordinateType = typename CartesianGridSampler<TPrimitive>::Coordinate;

    typename CartesianGridSampler<TPrimitive>::Point point;

    if (this->size() == 1) // center point
    {
        const CoordinateType denominator = 2;
        for (auto&& [r_component, r_base] : utils::zip(CartesianGridSampler<TPrimitive>::Dimension, point, r_primitive.base()))
            r_component = r_base + r_primitive.length() / denominator;
    }
    else
    {
        for (auto&& [r_component, r_index, r_base, r_resolution] : utils::zip(CartesianGridSampler<TPrimitive>::Dimension, point, this->getSpatialIndex(index), r_primitive.base(), this->getResolution()))
            r_component = r_base + r_index * r_primitive.length() / (r_resolution - 1);
    }

    return point;

    CIE_END_EXCEPTION_TRACING
}



/* --- Box sampler --- */

template <concepts::Box TPrimitive>
CartesianGridSampler<TPrimitive>::CartesianGridSampler(const Resolution& r_resolution) :
    AbsCartesianGridSampler<TPrimitive>(r_resolution)
{
}

template <concepts::Box TPrimitive>
inline typename CartesianGridSampler<TPrimitive>::Point
CartesianGridSampler<TPrimitive>::getSamplePoint(const TPrimitive& r_primitive,
                                                 Size index) const
{
    CIE_BEGIN_EXCEPTION_TRACING
    CIE_PROFILE_SCOPE

    using CoordinateType = typename CartesianGridSampler<TPrimitive>::Coordinate;
    typename CartesianGridSampler<TPrimitive>::Point point;

    if (this->size() == 1) // center point
    {
        constexpr CoordinateType denominator = 2;
        for (auto&& [r_component, r_base, r_length] : utils::zip(CartesianGridSampler<TPrimitive>::Dimension, point, r_primitive.base(), r_primitive.lengths()))
            r_component = r_base + r_length / denominator;
    }
    else
    {
        for (auto&& [r_component, r_index, r_base, r_length, r_resolution] : utils::zip(CartesianGridSampler<TPrimitive>::Dimension, point, this->getSpatialIndex(index), r_primitive.base(), r_primitive.lengths(), this->getResolution()))
            r_component = r_base + r_index * r_length / (r_resolution - 1);
    }

    return point;

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif