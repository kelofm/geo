#ifndef CIE_MESH_KERNEL_CARTESIAN_MESH_IMPL_HPP
#define CIE_MESH_KERNEL_CARTESIAN_MESH_IMPL_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/state_iterator.hpp"
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/macros/inc/exceptions.hpp"

// --- STL Includes ---
#include <algorithm>
#include <numeric>


namespace cie::geo {


template < concepts::NumericContainer TPoint,
           concepts::Container PointContainer >
requires std::derived_from<TPoint,typename GetTraitsT<TPoint>::Point>
void makeCartesianMesh( const typename GetTraitsT<TPoint>::Domain& r_domain,
                        const typename GetTraitsT<TPoint>::Resolution& r_resolution,
                        PointContainer& r_points )
{
    CIE_BEGIN_EXCEPTION_TRACING

    const Size dimension = GetTraitsT<TPoint>::Dimension;

    // Allocate memory for the points
    Size numberOfPoints = std::accumulate(
        r_resolution.begin(),
        r_resolution.end(),
        1.0,
        []( Size lhs, Size rhs ) -> Size { return lhs * rhs; }
    );

    utils::reserve( r_points, r_points.size() + numberOfPoints );

    // Get mesh origin
    TPoint origin;
    utils::resize( origin, dimension );
    for ( Size dim=0; dim<dimension; ++dim )
        origin[dim] = r_domain[dim].first;

    // Compute edge lengths
    TPoint edgeLengths;
    utils::resize( edgeLengths, dimension );
    for ( Size dim=0; dim<dimension; ++dim )
        edgeLengths[dim] = (r_domain[dim].second - r_domain[dim].first) / (r_resolution[dim] - 1);

    // Create iterator through the cartesian products
    std::vector<std::vector<Size>> cartesianStates( dimension );

    for ( Size dim=0; dim<dimension; ++dim )
    {
        auto& r_range = cartesianStates[dim];

        r_range.resize( r_resolution[dim] );

        std::iota( r_range.begin(),
                   r_range.end(),
                   0 );
    }

    auto cartesianProductIterator = utils::makeStateIterator( cartesianStates );

    // Construct mesh
    for ( Size pointIndex=0; pointIndex<numberOfPoints; ++pointIndex, ++cartesianProductIterator )
    {
        const auto& r_spatialIndex = *cartesianProductIterator;
        TPoint point = origin;

        for ( Size dim=0; dim<dimension; ++dim )
            point[dim] += (*r_spatialIndex[dim]) * edgeLengths[dim];

        detail::constructPrimitiveIntoContainer( r_points,
                                                 point );
    }

    CIE_END_EXCEPTION_TRACING
}


template < concepts::NumericContainer TPoint,
           concepts::Container PointContainer >
requires std::derived_from<TPoint,typename GetTraitsT<TPoint>::Point>
PointContainer makeCartesianMesh( const typename GetTraitsT<TPoint>::Domain& r_domain,
                                  const typename GetTraitsT<TPoint>::Resolution& r_resolution )
{
    CIE_BEGIN_EXCEPTION_TRACING

    PointContainer points;

    makeCartesianMesh<TPoint>(r_domain,
                              r_resolution,
                              points );

    return points;

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Cube TPrimitive,
           concepts::Container PrimitiveContainer >
void makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Resolution& r_numberOfPrimitives,
                        typename GetTraitsT<TPrimitive>::Coordinate edgeLength,
                        const typename TPrimitive::Point& r_origin,
                        PrimitiveContainer& r_primitives )
{
    CIE_BEGIN_EXCEPTION_TRACING

    // Allocate memory for the primitives
    Size numberOfPrimitives = std::accumulate(
        r_numberOfPrimitives.begin(),
        r_numberOfPrimitives.end(),
        1.0,
        []( Size lhs, Size rhs ) -> Size { return lhs * rhs; }
    );

    if ( numberOfPrimitives == 0 )
        return;

    utils::reserve( r_primitives, r_primitives.size() + numberOfPrimitives );

    // Create iterator through the cartesian products
    std::vector<std::vector<Size>> cartesianStates( GetTraitsT<TPrimitive>::Dimension );

    for ( Size dim=0; dim<GetTraitsT<TPrimitive>::Dimension; ++dim )
    {
        auto& r_range = cartesianStates[dim];

        r_range.resize( r_numberOfPrimitives[dim] );

        std::iota( r_range.begin(),
                   r_range.end(),
                   0 );
    }

    auto cartesianProductIterator = utils::makeStateIterator( cartesianStates );

    // Construct primitives
    for ( Size primitiveIndex=0; primitiveIndex<numberOfPrimitives; ++primitiveIndex, ++cartesianProductIterator )
    {
        const auto& r_spatialIndex = *cartesianProductIterator;
        auto base = r_origin;

        for ( Size dim=0; dim<GetTraitsT<TPrimitive>::Dimension; ++dim )
            base[dim] += (*r_spatialIndex[dim]) * edgeLength;

        detail::constructPrimitiveIntoContainer( r_primitives,
                                                 base, edgeLength );
    }

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Cube TPrimitive,
           concepts::Container PrimitiveContainer >
PrimitiveContainer makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Resolution& r_numberOfPrimitives,
                                      typename GetTraitsT<TPrimitive>::Coordinate edgeLength,
                                      const typename TPrimitive::Point& r_origin )
{
    CIE_BEGIN_EXCEPTION_TRACING

    PrimitiveContainer primitives;

    makeCartesianMesh<TPrimitive>( r_numberOfPrimitives,
                                      edgeLength,
                                      r_origin,
                                      primitives );

    return primitives;

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Box TPrimitive,
           concepts::Container PrimitiveContainer >
void makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Domain& r_domain,
                        const typename GetTraitsT<TPrimitive>::Resolution& r_resolution,
                        PrimitiveContainer& r_primitives )
{
    CIE_BEGIN_EXCEPTION_TRACING

    // Init
    const Size dimension = GetTraitsT<TPrimitive>::Dimension;
    using TPoint      = typename GetTraitsT<TPrimitive>::Point;

    // Allocate memory for the primitives
    Size numberOfPrimitives = std::accumulate(
        r_resolution.begin(),
        r_resolution.end(),
        1.0,
        []( Size lhs, Size rhs ) -> Size { return lhs * rhs; }
    );

    if ( numberOfPrimitives == 0 )
        return;

    utils::reserve( r_primitives, r_primitives.size() + numberOfPrimitives );

    // Compute edge lengths
    TPoint edgeLengths;
    utils::resize( edgeLengths, dimension );
    for ( Size dim=0; dim<dimension; ++dim )
        edgeLengths[dim] = (r_domain[dim].second - r_domain[dim].first) / r_resolution[dim];

    // Get mesh origin
    TPoint origin;
    utils::resize( origin, dimension );
    for ( Size dim=0; dim<dimension; ++dim )
        origin[dim] = r_domain[dim].first;

    // Create iterator through the cartesian products
    std::vector<std::vector<Size>> cartesianStates( dimension );

    for ( Size dim=0; dim<dimension; ++dim )
    {
        auto& r_range = cartesianStates[dim];

        r_range.resize( r_resolution[dim] );

        std::iota( r_range.begin(),
                   r_range.end(),
                   0 );
    }

    auto cartesianProductIterator = utils::makeStateIterator( cartesianStates );

    // Construct primitives
    for ( Size primitiveIndex=0; primitiveIndex<numberOfPrimitives; ++primitiveIndex, ++cartesianProductIterator )
    {
        const auto& r_spatialIndex = *cartesianProductIterator;
        auto base = origin;

        for ( Size dim=0; dim<GetTraitsT<TPrimitive>::Dimension; ++dim )
            base[dim] += (*r_spatialIndex[dim]) * edgeLengths[dim];

        detail::constructPrimitiveIntoContainer( r_primitives,
                                                 base, edgeLengths );
    }

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Box TPrimitive,
           concepts::Container PrimitiveContainer >
PrimitiveContainer makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Domain& r_domain,
                                      const typename GetTraitsT<TPrimitive>::Resolution& r_resolution )
{
    CIE_BEGIN_EXCEPTION_TRACING

    PrimitiveContainer primitives;

    makeCartesianMesh<TPrimitive>(r_domain,
                                  r_resolution,
                                  primitives );

    return primitives;

    CIE_END_EXCEPTION_TRACING
}





namespace detail {


template <concepts::NonPointerContainer ContainerType, class ...Args>
void constructPrimitiveIntoContainer( ContainerType& r_container,
                                      Args&&... args )
{
    CIE_BEGIN_EXCEPTION_TRACING

    r_container.emplace_back(
        std::forward<Args>(args)...
    );

    CIE_END_EXCEPTION_TRACING
}


template <concepts::PointerContainer ContainerType, class ...Args>
void constructPrimitiveIntoContainer( ContainerType& r_container,
                                      Args&&... args )
{
    CIE_BEGIN_EXCEPTION_TRACING

    r_container.emplace_back( new typename ContainerType::value_type::element_type(
        std::forward<Args>(args)...
    ));

    CIE_END_EXCEPTION_TRACING
}


} // namespace detail


} // namespace cie::geo


#endif