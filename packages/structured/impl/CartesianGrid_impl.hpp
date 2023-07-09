#ifndef CIE_MESH_CARTESIAN_GRID_IMPL_HPP
#define CIE_MESH_CARTESIAN_GRID_IMPL_HPP

// --- GEO Includes ---
#include "packages/structured/inc/CartesianGrid.hpp"

// --- Utility Includes ---
#include "packages/macros/inc/checks.hpp"


namespace cie::geo {


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
template <concepts::Iterator<TI> TIndexIt>
inline typename CartesianGrid<D,TC,TI,TE>::Point
CartesianGrid<D,TC,TI,TE>::getNode(TIndexIt it_index) const
{
    Point output;

    auto it_coordinate = output.begin();
    auto it_base = _base.begin();
    auto it_increment = _increments.begin();
    const TIndexIt it_end = it_index + D;

    for (; it_index!=it_end; ++it_index, ++it_base, ++it_increment, ++it_coordinate)
        *it_coordinate = (*it_base) + (*it_index) * (*it_increment);

    return output;
}


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
template <concepts::Container<TI> TContainer>
inline typename CartesianGrid<D,TC,TI,TE>::Point
CartesianGrid<D,TC,TI,TE>::getNode(const TContainer& r_indices) const
{
    CIE_OUT_OF_RANGE_CHECK(D <= r_indices.size())
    return this->getNode(r_indices.begin());
}


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
inline typename CartesianGrid<D,TC,TI,TE>::Point
CartesianGrid<D,TC,TI,TE>::getNode(TI index) const
{
    return this->getNode(_nodeConverter.convert(index));
}


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
template <concepts::Iterator<TI> TIndexIt>
inline typename CartesianGrid<D,TC,TI,TE>::Element
CartesianGrid<D,TC,TI,TE>::getElement(TIndexIt it_index) const
{
    Element element;
    element.first = this->getNode(it_index);
    element.second = _increments;
    return element;
}


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
template <concepts::Container<TI> TContainer>
inline typename CartesianGrid<D,TC,TI,TE>::Element
CartesianGrid<D,TC,TI,TE>::getElement(const TContainer& r_indices) const
{
    CIE_OUT_OF_RANGE_CHECK(D <= r_indices.size())
    return this->getElement(r_indices.begin());
}


template <Size D, concepts::Numeric TC, concepts::UnsignedInteger TI, class TE>
inline typename CartesianGrid<D,TC,TI,TE>::Element
CartesianGrid<D,TC,TI,TE>::getElement(TI index) const
{
    return this->getElement(_elementConverter.convert(index));
}


} // namespace cie::geo


#endif
