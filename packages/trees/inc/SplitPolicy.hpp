#ifndef CIE_GEO_SPLIT_POLICY_HPP
#define CIE_GEO_SPLIT_POLICY_HPP

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/iterator_concepts.hpp"

// --- Internal Includes ---
#include "packages/trees/inc/PrimitiveSampler.hpp"
#include "packages/macros/inc/typedefs.hpp"

// --- STL Includes ---
#include <memory>


namespace cie::geo {

/**
 * Interface for providing points around which
 * to split a cell. Generating the split point
 * is based on a set of point-value pairs and
 * must be implemented in derived classes.
*/
template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator>
class SplitPolicy
{
public:
    typedef PointIterator                       point_iterator_type;
    typedef ValueIterator                       value_iterator_type;
    typedef typename PointIterator::value_type  Point;
    typedef typename ValueIterator::value_type  value_type;

    CIE_DEFINE_CLASS_POINTERS(SplitPolicy)

public:
    virtual ~SplitPolicy() {}

    virtual Point operator()(  ValueIterator it_valueBegin,
                                    ValueIterator it_valueEnd,
                                    PointIterator it_pointBegin ) const = 0;
};



template <concepts::Iterator PointIterator,
          concepts::Iterator ValueIterator>
using SplitPolicyPtr = std::shared_ptr<SplitPolicy<PointIterator,ValueIterator>>;


} // namespace cie::geo

#endif