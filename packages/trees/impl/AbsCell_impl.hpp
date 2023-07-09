#ifndef CIE_GEO_TREES_ABS_CELL_IMPL_HPP
#define CIE_GEO_TREES_ABS_CELL_IMPL_HPP


namespace cie::geo {

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