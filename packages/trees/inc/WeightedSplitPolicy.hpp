#ifndef CIE_GEO_TREES_WEIGHTED_SPLIT_POLICY_HPP
#define CIE_GEO_TREES_WEIGHTED_SPLIT_POLICY_HPP

// --- Internal Includes ---
#include "packages/trees/inc/SplitPolicy.hpp"


namespace cie::geo {


/**
 * Compute a weighted average of the sample point set.
 * ( modified weights -> abs and reverse values )
*/
template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator >
class WeightedSplitPolicy : public SplitPolicy<PointIterator,ValueIterator>
{
public:
    virtual typename WeightedSplitPolicy<PointIterator,ValueIterator>::Point operator()(
        ValueIterator it_valueBegin,
        ValueIterator it_valueEnd,
        PointIterator it_pointBegin ) const override;
};


} // namespace cie::geo

#include "packages/trees/impl/WeightedSplitPolicy_impl.hpp"

#endif