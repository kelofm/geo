#ifndef CIE_GEO_TREES_ABS_CELL_IMPL_HPP
#define CIE_GEO_TREES_ABS_CELL_IMPL_HPP

// --- Geo Includes ---
#include "packages/trees/inc/AbsCell.hpp"


namespace cie::geo {


template <concepts::Primitive TPrimitive>
AbsCell<TPrimitive>::AbsCell(RightRef<TPrimitive> r_geometry) noexcept
    : TPrimitive(std::move(r_geometry))
{
}


template <concepts::Primitive TPrimitive>
AbsCell<TPrimitive>::AbsCell(Ref<const TPrimitive> r_geometry)
    : TPrimitive(r_geometry)
{
}


template <concepts::Primitive PrimitiveType>
template <class ...Args>
AbsCell<PrimitiveType>::AbsCell( Args&&... args ) :
    PrimitiveType( std::forward<Args>(args) ... )
{
}



template <concepts::Primitive PrimitiveType>
inline typename AbsCell<PrimitiveType>::primitive_constructor_container
AbsCell<PrimitiveType>::split( const typename AbsCell<PrimitiveType>::Point& point )
{
    return this->split_internal(point);
}


} // namespace cie::geo

#endif