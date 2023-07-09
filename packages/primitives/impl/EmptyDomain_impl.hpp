#ifndef CIE_GEO_EMPTY_DOMAIN_IMPL_HPP
#define CIE_GEO_EMPTY_DOMAIN_IMPL_HPP


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
inline Bool
EmptyDomain<Dimension,CoordinateType>::at( const typename EmptyDomain<Size,CoordinateType>::Point& r_point ) const
{
    return false;
}


} // namespace cie::geo


#endif