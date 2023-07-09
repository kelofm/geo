#ifndef CIE_GEO_TREES_WEIGHTED_SPLIT_POLICY_IMPL_HPP
#define CIE_GEO_TREES_WEIGHTED_SPLIT_POLICY_IMPL_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/zip.hpp"
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <algorithm>
#include <limits>


namespace cie::geo {


template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator>
inline typename WeightedSplitPolicy<PointIterator,ValueIterator>::Point
WeightedSplitPolicy<PointIterator,ValueIterator>::operator()(ValueIterator it_valueBegin,
                                                             ValueIterator it_valueEnd,
                                                             PointIterator it_pointBegin) const
{
    CIE_PROFILE_SCOPE
    using PointType = typename PointIterator::value_type;
    using CoordinateType = typename PointType::value_type;

    // Init
    CoordinateType absMaxValue = 0.0;
    CoordinateType weightSum = 0.0;
    CoordinateType weight;
    PointType splitPoint, min, max;

    std::fill(splitPoint.begin(),
              splitPoint.end(),
              0.0);
    std::fill(min.begin(),
              min.end(),
              std::numeric_limits<CoordinateType>::max());
    std::fill(max.begin(),
              max.end(),
              std::numeric_limits<CoordinateType>::min());

    // First pass: find max abs value
    for (ValueIterator it=it_valueBegin; it!=it_valueEnd; ++it)
        if (absMaxValue < std::abs(*it))
            absMaxValue = std::abs(*it);

    // Second pass: weight points and accumulate weights
    for (; it_valueBegin!=it_valueEnd; ++it_valueBegin,++it_pointBegin)
    {
        weight = absMaxValue - std::abs(*it_valueBegin);
        weightSum += weight;

        for (auto&& [r_component, r_splitComponent, r_min, r_max] : utils::zip(*it_pointBegin, splitPoint, min, max))
        {
            r_splitComponent += weight * r_component;

            if (r_component < r_min)
                r_min = r_component;
            if (r_max < r_component)
                r_max = r_component;
        }

    }

    // TODO: handle case where every value is 0
    if (weightSum == 0)
        weightSum = 1;

    for (auto&& [r_component, r_min, r_max] : utils::zip(splitPoint, min, max))
    {
        r_component /= weightSum;

        if (r_component < r_min)
            r_component = r_min;
        else if (r_max < r_component)
            r_component = r_max;
    }

    return splitPoint;
}


} // namespace cie::geo

#endif