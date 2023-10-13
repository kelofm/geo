#ifndef CIE_GEO_TREES_ABS_CELL_HPP
#define CIE_GEO_TREES_ABS_CELL_HPP

// --- Internal Includes ---
#include "packages/primitives/inc/Primitive.hpp"
#include "packages/primitives/inc/concepts.hpp"

// --- Utility Includes ---
#include "packages/trees/inc/abstree.hpp"
#include "packages/compile_time/packages/concepts/inc/basic_concepts.hpp"
#include "packages/compile_time/packages/concepts/inc/partial_specialization.hpp"
#include "packages/types/inc/types.hpp"

// --- STL Includes ---
#include <memory>
#include <vector>

namespace cie::geo {

/**
 * Node interface of a tree of primitives, splittable around a single point.
 * The 'split' interface supplies a set of tuples necessary for constructing
 * a set of primitives (results of the split).
*/
template <concepts::Primitive PrimitiveType>
class AbsCell : public PrimitiveType
{
public:
    using Primitive = PrimitiveType;
    using CellBase  = AbsCell<PrimitiveType>;

    using primitive_constructor_container       = std::vector<typename Primitive::primitive_constructor_arguments>;
    using primitive_constructor_container_ptr   = std::shared_ptr<primitive_constructor_container>;

public:
    AbsCell(RightRef<PrimitiveType> r_geometry) noexcept;

    AbsCell(Ref<const PrimitiveType> r_geometry);

    /**
     * Construct a cell by forwarding incoming arguments to the
     * primitive constructor.
    */
    template <class ...Args>
    AbsCell( Args&&... args );

    virtual ~AbsCell() {}

    /**
     * Split overload that doesn't attempt to convert the coordinate container
     * to Point (as it is already Point).
    */
    primitive_constructor_container split( const typename AbsCell<PrimitiveType>::Point& point );

protected:

    /**
     * Split the primitive around a point and return a container of tuples
     * that hold the arguments for constructing the subcells.
    */
    virtual primitive_constructor_container split_internal( const typename AbsCell<PrimitiveType>::Point& point ) = 0;
};


CIE_DEFINE_INVALID_CLASS_TEMPLATE_TO_SPECIALIZE( Cell )


} // namespace cie::geo

#include "packages/trees/impl/AbsCell_impl.hpp"

#endif