#ifndef CIE_MESH_KERNEL_CARTESIAN_MESH_HPP
#define CIE_MESH_KERNEL_CARTESIAN_MESH_HPP

// --- GEO Includes ---
#include "packages/primitives/inc/Cube.hpp"
#include "packages/primitives/inc/Box.hpp"

// --- Utility Includes ---
#include "packages/compile_time/packages/concepts/inc/container_concepts.hpp"

// --- Internal Includes ---
#include "packages/traits/inc/MeshTraits.hpp"

// --- STL Includes ---
#include <vector>
#include <functional>


namespace cie::geo {


/// Generate a cartesian mesh of points into an existing container
template < concepts::NumericContainer TPoint,
           concepts::Container PointContainer >
requires std::derived_from<TPoint,typename GetTraitsT<TPoint>::Point>
void makeCartesianMesh( const typename GetTraitsT<TPoint>::Domain& r_domain,
                        const typename GetTraitsT<TPoint>::Resolution& r_resolution,
                        PointContainer& r_points );


/// Generate a cartesian mesh of points
template < concepts::NumericContainer TPoint,
           concepts::Container PointContainer = std::vector<TPoint> >
requires std::derived_from<TPoint,typename GetTraitsT<TPoint>::Point>
PointContainer makeCartesianMesh( const typename GetTraitsT<TPoint>::Domain& r_domain,
                                  const typename GetTraitsT<TPoint>::Resolution& r_resolution );


/// Generate a cartesian mesh of cube elements into an existing container
template < concepts::Cube TPrimitive,
           concepts::Container PrimitiveContainer >
void makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Resolution& r_numberOfPrimitives,
                        typename GetTraitsT<TPrimitive>::Coordinate edgeLength,
                        const typename TPrimitive::Point& r_origin,
                        PrimitiveContainer& r_primitives );


/// Generate a cartesian mesh of cube elements
template < concepts::Cube TPrimitive,
           concepts::Container PrimitiveContainer = std::vector<std::shared_ptr<TPrimitive>> >
PrimitiveContainer makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Resolution& r_numberOfPrimitives,
                                      typename GetTraitsT<TPrimitive>::Coordinate edgeLength,
                                      const typename TPrimitive::Point& r_origin = geo::detail::makeOrigin<GetTraitsT<TPrimitive>::Dimension,typename GetTraitsT<TPrimitive>::Coordinate>() );


/// Generate a cartesian mesh of box elements into an existing container
template < concepts::Box TPrimitive,
           concepts::Container PrimitiveContainer >
void makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Domain& r_domain,
                        const typename GetTraitsT<TPrimitive>::Resolution& r_resolution,
                        PrimitiveContainer& r_primitives );


/// Generate a cartesian mesh of box elements
template < concepts::Box TPrimitive,
           concepts::Container PrimitiveContainer = std::vector<std::shared_ptr<TPrimitive>> >
PrimitiveContainer makeCartesianMesh( const typename GetTraitsT<TPrimitive>::Domain& r_domain,
                                      const typename GetTraitsT<TPrimitive>::Resolution& r_resolution );





// Helper functions for emplacing back
// primitives or primitive pointers into a container
namespace detail {

template <concepts::NonPointerContainer ContainerType, class ...Args>
void constructPrimitiveIntoContainer( ContainerType& r_container,
                                      Args&&... args );

template <concepts::PointerContainer ContainerType, class ...Args>
void constructPrimitiveIntoContainer( ContainerType& r_container,
                                      Args&&... args );

} // namespace detail


} // namespace cie::geo

#include "packages/structured/impl/cartesianmesh_impl.hpp"

#endif