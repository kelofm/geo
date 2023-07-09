#ifndef CIE_GEO_TREES_MID_POINT_SPLIT_POLICY_HPP
#define CIE_GEO_TREES_MID_POINT_SPLIT_POLICY_HPP

// --- Internal Includes ---
#include "packages/trees/inc/SplitPolicy.hpp"


namespace cie::geo {


/// @brief Compute the arithmetic mean of the input points.
/// @ingroup geo.
template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator>
class MidPointSplitPolicy : public SplitPolicy<PointIterator,ValueIterator>
{
public:
    virtual typename MidPointSplitPolicy<PointIterator,ValueIterator>::Point operator()(
                ValueIterator it_valueBegin,
                ValueIterator it_valueEnd,
                PointIterator it_pointBegin) const override;
};


} // namespace cie::geo

#include "packages/trees/impl/MidPointSplitPolicy_impl.hpp"

#endif