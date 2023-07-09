#ifndef CIE_GEO_TREES_CORNER_SAMPLE_IMPL_HPP
#define CIE_GEO_TREES_CORNER_SAMPLE_IMPL_HPP

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"
#include "packages/stl_extension/inc/zip.hpp"


namespace cie::geo {


template <concepts::Cube PrimitiveType>
CornerSampler<PrimitiveType>::CornerSampler()
{
    CIE_BEGIN_EXCEPTION_TRACING

    typename IndexConverter::Resolution resolution;
    for (auto& r_resolution : resolution)
        r_resolution = 2;

    _p_indexConverter.reset(new IndexConverter(resolution));

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Cube PrimitiveType>
inline Size
CornerSampler<PrimitiveType>::size() const
{
    return _p_indexConverter->size();
}


template <concepts::Cube PrimitiveType>
typename PrimitiveType::Point
CornerSampler<PrimitiveType>::getSamplePoint(const PrimitiveType& r_primitive,
                                             const Size index) const
{
    CIE_BEGIN_EXCEPTION_TRACING
    CIE_PROFILE_SCOPE

    auto point = r_primitive.base();

    for (auto&& [r_component, r_index] : utils::zip(point, _p_indexConverter->convert(index)))
        r_component += r_index * r_primitive.length();

    return point;

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Box PrimitiveType>
CornerSampler<PrimitiveType>::CornerSampler()
{
    CIE_BEGIN_EXCEPTION_TRACING

    typename IndexConverter::Resolution resolution;
    for (auto& r_resolution : resolution)
        r_resolution = 2;

    _p_indexConverter.reset(new IndexConverter(resolution));

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Box PrimitiveType>
inline Size
CornerSampler<PrimitiveType>::size() const
{
    return _p_indexConverter->size();
}


template <concepts::Box PrimitiveType>
typename PrimitiveType::Point
CornerSampler<PrimitiveType>::getSamplePoint(const PrimitiveType& r_primitive,
                                             const Size index) const
{
    CIE_BEGIN_EXCEPTION_TRACING
    CIE_PROFILE_SCOPE

    auto point = r_primitive.base();

    for (auto&& [r_component, r_index, r_length] : utils::zip(point, _p_indexConverter->convert(index), r_primitive.lengths()))
        r_component += r_index * r_length;

    return point;

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif