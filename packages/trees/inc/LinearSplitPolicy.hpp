#ifndef CIE_GEO_TREES_LINEAR_SPLIT_POLICY_HPP
#define CIE_GEO_TREES_LINEAR_SPLIT_POLICY_HPP

// --- Internal Includes ---
#include "packages/trees/inc/SplitPolicy.hpp"


namespace cie::geo {


/**
 * Find the root of the least squares line of each component
 * and bound it by their limits. 
*/
template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator>
class LinearSplitPolicy : public SplitPolicy<PointIterator,ValueIterator>
{
public:
    virtual typename LinearSplitPolicy<PointIterator,ValueIterator>::Point operator()(
        ValueIterator it_valueBegin,
        ValueIterator it_valueEnd,
        PointIterator it_pointBegin ) const override;
};


} // namespace cie::geo

#include "packages/trees/impl/LinearSplitPolicy_impl.hpp"

#endif