#ifndef CIE_GEO_OBJECT_WRAPPER_IMPL_HPP
#define CIE_GEO_OBJECT_WRAPPER_IMPL_HPP


namespace cie::geo {


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
ObjectWrapper<Dimension,ValueType,CoordinateType>::ObjectWrapper( typename ObjectWrapper<Dimension,ValueType,CoordinateType>::functor_type functor ) :
    _functor( functor )
{
}


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
inline ValueType
ObjectWrapper<Dimension,ValueType,CoordinateType>::at( const typename ObjectWrapper<Dimension,ValueType,CoordinateType>::Point& r_point ) const
{
    return this->_functor( r_point );
}


} // namespace cie::geo


#endif