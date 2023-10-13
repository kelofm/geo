#ifndef CIE_GEO_TREES_CELL_HPP
#define CIE_GEO_TREES_CELL_HPP

// --- GEO Includes ---
#include "packages/trees/inc/AbsCell.hpp"
#include "packages/primitives/inc/Cube.hpp"
#include "packages/primitives/inc/Box.hpp"
#include "packages/trees/inc/CartesianIndexConverter.hpp"


namespace cie::geo {


template <concepts::Cube PrimitiveType>
class Cell<PrimitiveType> : public AbsCell<PrimitiveType>
{
private:
    using IndexConverter = CartesianIndexConverter<PrimitiveType::Dimension,Size,Size,tags::Eager>;

public:
    Cell( const typename Cell<PrimitiveType>::Point& base,
          typename Cell<PrimitiveType>::Coordinate length );

    using AbsCell<PrimitiveType>::AbsCell;

    typename Cell<PrimitiveType>::primitive_constructor_container split( const typename Cell<PrimitiveType>::Point& r_point );

    /**
     * A cube will only produce subcubes when split if it's split at the
     * midpoint, so the actual split point is unnecessary and ignored.
    */
    typename Cell<PrimitiveType>::primitive_constructor_container split( );

protected:
    virtual typename Cell<PrimitiveType>::primitive_constructor_container split_internal( const typename Cell<PrimitiveType>::Point& point ) override;

protected:
    static const IndexConverter _childIndexConverter;
};



/* --- Box cell --- */

template <concepts::Box PrimitiveType>
class Cell<PrimitiveType> : public AbsCell<PrimitiveType>
{
private:
    using IndexConverter = CartesianIndexConverter<PrimitiveType::Dimension,Size,Size,tags::Eager>;

public:
    Cell( const typename Cell<PrimitiveType>::Point& base,
          const typename Cell<PrimitiveType>::Point& lengths );

    using AbsCell<PrimitiveType>::AbsCell;

protected:
    virtual typename Cell<PrimitiveType>::primitive_constructor_container split_internal( const typename Cell<PrimitiveType>::Point& point ) override;

protected:
    static const IndexConverter _childIndexConverter;
};


} // namespace cie::geo

#include "packages/trees/impl/Cell_impl.hpp"

#endif
