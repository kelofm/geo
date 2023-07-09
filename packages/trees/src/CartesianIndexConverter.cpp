// --- GEO Includes ---
#include "packages/trees/inc/CartesianIndexConverter.hpp"

// --- STL Includes ---
#include <numeric>


namespace cie::geo {


namespace detail {


namespace {
template <class TConverter>
typename TConverter::Resolution getUniformSamplingResolution(Size samplingOrder) noexcept
{
    typename TConverter::Resolution resolution;
    std::fill(resolution.begin(), resolution.end(), samplingOrder);
    return resolution;
}
} // unnamed namespace


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat>
CartesianIndexConverter<Dim,TIndex,TFlat,tags::Eager>::CartesianIndexConverter(const Resolution& r_resolution)
{
    CIE_BEGIN_EXCEPTION_TRACING

    const TFlat numberOfIndices = std::accumulate(r_resolution.begin(),
                                                  r_resolution.end(),
                                                  1,
                                                  [](TFlat lhs, TIndex rhs) -> TFlat {return lhs * rhs;});

    if (numberOfIndices)
    {
        // Init
        _indexMap.emplace_back();
        std::fill(_indexMap.back().begin(),
                _indexMap.back().end(),
                0);

        if (Dim)
        {
            // Initialize the rest of the first dimension.
            for (TIndex flatIndex=1; flatIndex<r_resolution[0]; ++flatIndex)
            {
                _indexMap.push_back(_indexMap.back());
                ++_indexMap.back()[0];
            }

            TFlat prefixPopulation = r_resolution[0]; // Keeps track of how many nodes there are in lower dimensions per level.

            // Fill the rest of the map, building on previous indices.
            for (TIndex dimension=1; dimension<Dim; ++dimension)
            {
                const auto localResolution = r_resolution[dimension];

                for (TIndex localIndex=1; localIndex<localResolution; ++localIndex)
                {
                    for (TFlat prefixIndex=0; prefixIndex<prefixPopulation; ++prefixIndex)
                    {
                        _indexMap.push_back(_indexMap[_indexMap.size() - prefixPopulation]);
                        ++_indexMap.back()[dimension];
                    }
                }

                prefixPopulation *= localResolution;
            } // for dimension in range(Dim)
        } // if Dim
    } // if numberOfIndices

    CIE_END_EXCEPTION_TRACING
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat>
CartesianIndexConverter<Dim,TIndex,TFlat,tags::Lazy>::CartesianIndexConverter(const Resolution& r_resolution)
{
    CIE_BEGIN_EXCEPTION_TRACING

    std::exclusive_scan(r_resolution.begin(),
                        r_resolution.end(),
                        _prefixPopulation.begin(),
                        1,
                        [](Flat lhs, Flat rhs) -> Flat {return lhs * rhs;});

    _prefixPopulation.back() = _prefixPopulation[Dim - 1] * r_resolution.back();

    for (TIndex resolution : _prefixPopulation)
        CIE_DIVISION_BY_ZERO_CHECK(resolution != 0)

    CIE_END_EXCEPTION_TRACING
}


} // namespace detail


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat, class ExecutionTag>
CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>::CartesianIndexConverter(const Resolution& r_resolution)
    : _converter(r_resolution)
{
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat, class ExecutionTag>
CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>::CartesianIndexConverter(TIndex pointsPerDirection)
    : CartesianIndexConverter(detail::getUniformSamplingResolution<CartesianIndexConverter>(pointsPerDirection))
{
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat, class ExecutionTag>
template <concepts::Iterator<TIndex> TBegin, concepts::Iterator<TIndex> TEnd>
void
CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>::setResolution(TBegin begin, TEnd end)
{
    CIE_BEGIN_EXCEPTION_TRACING

    Resolution resolution;
    CIE_OUT_OF_RANGE_CHECK(std::distance(begin, end) < Dim)
    std::copy(begin, end, resolution.begin());

    this->setResolution(resolution);

    CIE_END_EXCEPTION_TRACING
}


template <Size Dim, concepts::UnsignedInteger TIndex, concepts::UnsignedInteger TFlat, class ExecutionTag>
void
CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>::setResolution(const Resolution& r_resolution)
{
    CIE_BEGIN_EXCEPTION_TRACING
    _converter = detail::CartesianIndexConverter<Dim,TIndex,TFlat,ExecutionTag>(r_resolution);
    CIE_END_EXCEPTION_TRACING
}


// --- Template Instantiations ---

#define CIE_LOCAL_SUBSTITUTE_DIMENSIONS(TINDEX, TFLAT, TTAG) \
    template class CartesianIndexConverter<1,TINDEX,TFLAT,TTAG>; \
    template class CartesianIndexConverter<2,TINDEX,TFLAT,TTAG>; \
    template class CartesianIndexConverter<3,TINDEX,TFLAT,TTAG>;

#define CIE_LOCAL_SUBSTITUTE_INDEX(TFLAT,TTAG) \
    CIE_LOCAL_SUBSTITUTE_DIMENSIONS(unsigned short, TFLAT, TTAG) \
    CIE_LOCAL_SUBSTITUTE_DIMENSIONS(unsigned char, TFLAT, TTAG) \
    CIE_LOCAL_SUBSTITUTE_DIMENSIONS(unsigned int, TFLAT, TTAG) \
    CIE_LOCAL_SUBSTITUTE_DIMENSIONS(unsigned long, TFLAT, TTAG) \
    CIE_LOCAL_SUBSTITUTE_DIMENSIONS(unsigned long long, TFLAT, TTAG)

#define CIE_LOCAL_SUBSTITUTE_FLAT(TTAG) \
    CIE_LOCAL_SUBSTITUTE_INDEX(unsigned short, TTAG) \
    CIE_LOCAL_SUBSTITUTE_INDEX(unsigned char, TTAG) \
    CIE_LOCAL_SUBSTITUTE_INDEX(unsigned int, TTAG) \
    CIE_LOCAL_SUBSTITUTE_INDEX(unsigned long, TTAG) \
    CIE_LOCAL_SUBSTITUTE_INDEX(unsigned long long, TTAG)

CIE_LOCAL_SUBSTITUTE_FLAT(tags::Eager)

CIE_LOCAL_SUBSTITUTE_FLAT(tags::Lazy)

#undef CIE_LOCAL_SUBSTITUTE_FLAT
#undef CIE_LOCAL_SUBSTITUTE_INDEX
#undef CIE_LOCAL_SUBSTITUTE_DIMENSIONS


} // namespace cie::geo
