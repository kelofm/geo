#ifndef CIE_GEO_TREES_LINEAR_SPLIT_POLICY_IMPL_HPP
#define CIE_GEO_TREES_LINEAR_SPLIT_POLICY_IMPL_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <numeric>
#include <algorithm>
#include <limits>


namespace cie::geo {


template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator>
inline typename LinearSplitPolicy<PointIterator,ValueIterator>::Point
LinearSplitPolicy<PointIterator,ValueIterator>::operator()( ValueIterator it_valueBegin,
                                                            ValueIterator it_valueEnd,
                                                            PointIterator it_pointBegin ) const
{
    CIE_PROFILE_SCOPE
    using PointType         = typename PointIterator::value_type;
    using CoordinateType    = typename PointType::value_type;

    // Init
    CoordinateType valueSum = std::accumulate(  it_valueBegin,
                                                 it_valueEnd,
                                                 0 );
    CoordinateType numberOfPoints = std::distance(  it_valueBegin,
                                                    it_valueEnd );

    // Initialize other components
    PointType componentSums, componentSquaredSums, mixedSums, min, max;
    std::fill(  componentSums.begin(),
                componentSums.end(),
                0 );
    std::fill(  componentSquaredSums.begin(),
                componentSquaredSums.end(),
                0 );
    std::fill(  mixedSums.begin(),
                mixedSums.end(),
                0 );
    std::fill(  min.begin(),
                min.end(),
                std::numeric_limits<CoordinateType>::max() );
    std::fill(  max.begin(),
                max.end(),
                std::numeric_limits<CoordinateType>::min() );

    typename PointType::iterator
        it_componentSums, it_componentSquaredSums, it_mixedSums, it_min, it_max;

    // Loop through points (and values)
    for ( ; it_valueBegin!=it_valueEnd; ++it_valueBegin,++it_pointBegin )
    {
        it_componentSums        = componentSums.begin();
        it_componentSquaredSums = componentSquaredSums.begin();
        it_mixedSums            = mixedSums.begin();
        it_min                  = min.begin();
        it_max                  = max.begin();

        // Loop through components of the current point
        for ( const auto& component : *it_pointBegin )
        {
            *it_componentSums++         += component;
            *it_componentSquaredSums++  += component * component;
            *it_mixedSums++             += component * (*it_valueBegin);

            if (component < *it_min)
                *it_min = component;
            if (*it_max < component)
                *it_max = component;

            ++it_min;
            ++it_max;
        }
    }

    // Compute and restrict split point componentwise
    PointType splitPoint;
    it_componentSums        = componentSums.begin();
    it_componentSquaredSums = componentSquaredSums.begin();
    it_mixedSums            = mixedSums.begin();
    it_min = min.begin();
    it_max = max.begin();

    for ( auto& r_component : splitPoint )
    {
        r_component = (*it_mixedSums) - (*it_componentSums)*valueSum / numberOfPoints;  // <-- denominator
        if (r_component == 0)                                                           // <-- horizontal line -> get midpoint for this component
            r_component = *it_componentSums / numberOfPoints;
        else
        {
            r_component =
            (
                (*it_componentSquaredSums) - (*it_componentSums)*(*it_componentSums) / numberOfPoints
            ) / r_component;
            r_component = ( (*it_componentSums) - valueSum * r_component ) / numberOfPoints;
        }

        if ( r_component < *it_min )
            r_component = *it_min;
        if ( *it_max < r_component )
            r_component = *it_max;

        ++it_componentSums;
        ++it_componentSquaredSums;
        ++it_mixedSums;
        ++it_min;
        ++it_max;
    }

    return splitPoint;
}



} // namespace cie::geo

#endif