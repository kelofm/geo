#ifndef CIE_GEO_TREES_MID_POINT_SPLIT_POLICY_IMPL_HPP
#define CIE_GEO_TREES_MID_POINT_SPLIT_POLICY_IMPL_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <algorithm>


namespace cie::geo {


template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator>
inline typename MidPointSplitPolicy<PointIterator,ValueIterator>::Point
MidPointSplitPolicy<PointIterator,ValueIterator>:: operator()(
            ValueIterator it_valueBegin,
            ValueIterator it_valueEnd,
            PointIterator it_pointBegin ) const
{
    CIE_PROFILE_SCOPE
    using PointType         = typename PointIterator::value_type;
    using CoordinateType    = typename PointType::value_type;

    PointType splitPoint;

    cie::utils::resize(splitPoint, (*it_pointBegin).size());
    auto it_splitPointBegin = splitPoint.begin();
    const auto it_splitPointEnd   = splitPoint.end();

    std::fill(it_splitPointBegin,
              it_splitPointEnd,
              0.0);


    CoordinateType numberOfPoints = std::distance(it_valueBegin, it_valueEnd);

    for (; it_valueBegin!=it_valueEnd; ++it_valueBegin,++it_pointBegin)
    {
        auto point = *it_pointBegin;
        auto it_tmp = point.begin();
        for (auto it_coordinate=it_splitPointBegin; it_coordinate!=it_splitPointEnd; ++it_coordinate,++it_tmp)
            *it_coordinate += *it_tmp;
    }

    for (auto& r_coordinate : splitPoint)
        r_coordinate /= numberOfPoints;

    return splitPoint;
}


} // namespace cie::geo

#endif