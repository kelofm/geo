#ifndef CIE_GEO_OBJECT_WRAPPER_HPP
#define CIE_GEO_OBJECT_WRAPPER_HPP

// --- Internal Includes ---
#include "packages/primitives/inc/Object.hpp"

// --- STL Includes ---
#include <functional>


namespace cie::geo {


template < Size Dimension,
           concepts::CopyConstructible ValueType,
           concepts::Numeric CoordinateType >
class ObjectWrapper : public Object<Dimension,ValueType,CoordinateType>
{
public:
    using typename Object<Dimension,ValueType,CoordinateType>::Point;
    using functor_type = std::function<ValueType(const Point&)>;

public:
    ObjectWrapper( functor_type functor );
    ObjectWrapper( const ObjectWrapper<Dimension,ValueType,CoordinateType>& r_rhs ) = default;
    ObjectWrapper( ObjectWrapper<Dimension,ValueType,CoordinateType>&& r_rhs ) = default;
    ObjectWrapper<Dimension,ValueType,CoordinateType>& operator=( const ObjectWrapper<Dimension,ValueType,CoordinateType>& r_rhs ) = default;

    virtual ValueType at( const Point& r_point ) const override;

private:
    ObjectWrapper() = delete;

protected:
    functor_type _functor;
};


} // namespace cie::geo

#include "packages/primitives/impl/ObjectWrapper_impl.hpp"

#endif