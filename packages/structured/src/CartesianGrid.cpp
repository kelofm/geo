// --- Mesh Includes ---
#include "packages/structured/inc/CartesianGrid.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <algorithm>
#include <ranges>


namespace cie::geo {


namespace {


template <class TGrid, class TResolutionIt>
typename TGrid::Resolution getElementResolution(TResolutionIt it_nodeResolution)
{
    typename TGrid::Resolution output;
    auto end = output.end();
    for (auto it=output.begin(); it!=end; ++it, ++it_nodeResolution)
    {
        if (*it_nodeResolution)
            *it = *it_nodeResolution - 1;
        else
            *it = 0;
    }

    return output;
}


} // unnamed namespace


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
template <concepts::Iterator<TC> TBaseIt,
          concepts::Iterator<TC> TLengthIt,
          concepts::Iterator<TI> TResolutionIt>
CartesianGrid<D,TC,TI,TE>::CartesianGrid(TBaseIt it_base,
                                         TLengthIt it_length,
                                         TResolutionIt it_resolution)
    : _nodeConverter(Resolution(it_resolution, it_resolution + D)),
      _elementConverter(getElementResolution(it_resolution)),
      _resolution(Resolution(it_resolution, it_resolution + D))
{
    std::copy(it_base, it_base + D, _base.begin());

    #ifdef CIE_ENABLE_RUNTIME_GEOMETRY_CHECKS
        for (Size dimension=0; dimension<D; ++dimension)
            CIE_CHECK(0 <= *(it_length + dimension), "Negative edge length (" << (*(it_length + dimension)) << " at index " << dimension)
    #endif

    if (std::all_of(it_resolution, it_resolution + D, [](TI resolution) {return resolution;}))
    {
        std::transform(it_length,
                       it_length + D,
                       it_resolution,
                       _increments.begin(),
                       [](TC length, TI resolution) -> TC {return length / resolution;});
    }
    else
    {
        std::fill(_increments.begin(), _increments.end(), 0);
    }
}


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
template <concepts::Container<TC> BaseContainer, concepts::Container<TC> LengthContainer>
CartesianGrid<D,TC,TI,TE>::CartesianGrid(const BaseContainer& r_base,
                                         const LengthContainer& r_lengths,
                                         const Resolution& r_resolution)
    : CartesianGrid(r_base.begin(), r_lengths.begin(), r_resolution.begin())
{
}



} // namespace cie::geo
