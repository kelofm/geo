#ifndef CIE_GEO_TREES_CARTESIAN_INDEX_CONVERTER_IMPL_HPP
#define CIE_GEO_TREES_CARTESIAN_INDEX_CONVERTER_IMPL_HPP

// --- GEO Includes ---
#include "packages/trees/inc/CartesianIndexConverter.hpp"

// --- Utility Includes ---
#include "packages/maths/inc/base.hpp"
#include "packages/maths/inc/power.hpp"
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/checks.hpp"

// --- STL Includes ---
#include <iterator>
#include <numeric>


namespace cie::geo {


namespace detail {


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat>
inline typename CartesianIndexConverter<Dim,TIndex,TFlat,tags::Eager>::ReturnType
CartesianIndexConverter<Dim,TIndex,TFlat,tags::Eager>::convert(Flat index) const
{
    CIE_OUT_OF_RANGE_CHECK(index < this->size())
    return _indexMap[index];
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat>
inline Size
CartesianIndexConverter<Dim,TIndex,TFlat,tags::Eager>::size() const noexcept
{
    return _indexMap.size();
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat>
inline typename CartesianIndexConverter<Dim,TIndex,TFlat,tags::Lazy>::ReturnType
CartesianIndexConverter<Dim,TIndex,TFlat,tags::Lazy>::convert(Flat index) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    SpatialIndex output;

    for (TIndex reverseDimension=0; reverseDimension<Dim; ++reverseDimension)
    {
        const TIndex dimension = Dim - reverseDimension - 1;
        const TFlat prefixPopulation = _prefixPopulation[dimension];
        const TIndex localIndex = index / prefixPopulation;
        output[dimension] = localIndex;

        index = index % prefixPopulation;
    }

    return output;

    CIE_END_EXCEPTION_TRACING
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat>
inline Size
CartesianIndexConverter<Dim,TIndex,TFlat,tags::Lazy>::size() const noexcept
{
    return _prefixPopulation.back();
}


} // namespace detail


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat, class ExecutionTag>
inline typename CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>::ReturnType
CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>::convert(TFlat index) const
{
    CIE_BEGIN_EXCEPTION_TRACING
    return _converter.convert(index);
    CIE_END_EXCEPTION_TRACING
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat, class ExecutionTag>
inline Size
CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>::size() const noexcept
{
    return _converter.size();
}


} // namespace cie::geo

#endif