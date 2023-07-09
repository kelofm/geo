#ifndef CIE_GEO_INFINITE_DOMAIN_IMPL_HPP
#define CIE_GEO_INFINITE_DOMAIN_IMPL_HPP


namespace cie::geo {


template <Size Dimension, concepts::Numeric CoordinateType>
inline Bool
InfiniteDomain<Dimension,CoordinateType>::at( const typename InfiniteDomain<Size,CoordinateType>::Point& r_point ) const
{
    return true;
}


} // namespace cie::geo


#endif