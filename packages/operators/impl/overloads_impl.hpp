#ifndef CIE_GEO_OVERLOADS_IMPL_HPP
#define CIE_GEO_OVERLOADS_IMPL_HPP


namespace cie::geo {


template <Size Dimension, concepts::Numeric TCoordinate>
ObjectPtr<Dimension,Bool,TCoordinate> operator*(ObjectPtr<Dimension,Bool,TCoordinate> p_lhs,
                                                ObjectPtr<Dimension,Bool,TCoordinate> p_rhs)
{
    return ObjectPtr<Dimension,Bool,TCoordinate>(
        new Intersection<Dimension,TCoordinate>(p_lhs, p_rhs)
    );
}


template <Size Dimension, concepts::Numeric TCoordinate>
ObjectPtr<Dimension,Bool,TCoordinate> operator+(ObjectPtr<Dimension,Bool,TCoordinate> p_lhs,
                                                ObjectPtr<Dimension,Bool,TCoordinate> p_rhs)
{
    return ObjectPtr<Dimension,Bool,TCoordinate>(
        new Union<Dimension,TCoordinate>(p_lhs, p_rhs)
    );
}


template <Size Dimension, concepts::Numeric TCoordinate>
ObjectPtr<Dimension,Bool,TCoordinate> operator-(ObjectPtr<Dimension,Bool,TCoordinate> p_lhs,
                                                ObjectPtr<Dimension,Bool,TCoordinate> p_rhs)
{
    return ObjectPtr<Dimension,Bool,TCoordinate>(
        new Subtraction<Dimension,TCoordinate>(p_lhs, p_rhs)
    );
}


} // namespace cie::geo


#endif