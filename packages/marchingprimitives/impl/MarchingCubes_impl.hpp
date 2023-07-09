#ifndef CIE_MESH_KERNEL_MARCHING_CUBES_IMPL_HPP
#define CIE_MESH_KERNEL_MARCHING_CUBES_IMPL_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/state_iterator.hpp"
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/checks.hpp"
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/maths/inc/bit.hpp"

// --- Internal Includes ---
#include "packages/marchingprimitives/inc/connectivitytables.hpp"

// --- STL Includes ---
#include <algorithm>
#include <ranges>


namespace cie::geo {


namespace detail {
template <concepts::Cube TPrimitive>
inline typename TPrimitive::Point
getVertexOnPrimitive( const TPrimitive& r_primitive,
                      Size vertexIndex )
{
    CIE_BEGIN_EXCEPTION_TRACING

    auto vertex = r_primitive.base();

    for ( Size bitIndex=0; bitIndex<TPrimitive::Dimension; ++bitIndex )
        if ( utils::getBit(vertexIndex,bitIndex) )
            vertex[bitIndex] += r_primitive.length();

    return vertex;

    CIE_END_EXCEPTION_TRACING
}

template <concepts::Box TPrimitive>
inline typename TPrimitive::Point
getVertexOnPrimitive( const TPrimitive& r_primitive,
                      Size vertexIndex )
{
    CIE_BEGIN_EXCEPTION_TRACING

    auto vertex = r_primitive.base();

    for ( Size bitIndex=0; bitIndex<TPrimitive::Dimension; ++bitIndex )
        if ( utils::getBit(vertexIndex,bitIndex) )
            vertex[bitIndex] += r_primitive.lengths()[bitIndex];

    return vertex;

    CIE_END_EXCEPTION_TRACING
}
} // namespace detail


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
UnstructuredMarchingCubes<TTarget,TPrimitive>::UnstructuredMarchingCubes(target_ptr p_target,
                                                                         primitive_container_ptr p_primitives,
                                                                         output_functor outputFunctor ) :
    UnstructuredMarchingPrimitives<TTarget,TPrimitive>(
        p_target,
        detail::getCubeEdgeMap<UnstructuredMarchingCubes::Dimension>(),
        detail::getCubeConnectivityMap<UnstructuredMarchingCubes::Dimension>(),
        outputFunctor,
        p_primitives )
{
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
inline Size
UnstructuredMarchingCubes<TTarget,TPrimitive>::primitiveVertexCount() const
{
    return intPow(2, UnstructuredMarchingCubes<TTarget,TPrimitive>::Dimension);
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
inline typename UnstructuredMarchingCubes<TTarget,TPrimitive>::Point
UnstructuredMarchingCubes<TTarget,TPrimitive>::getVertexOnPrimitive(const TPrimitive& r_primitive,
                                                                    Size vertexIndex) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    return detail::getVertexOnPrimitive( r_primitive, vertexIndex );

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
StructuredMarchingCubes<TTarget,TPrimitive>::StructuredMarchingCubes( typename StructuredMarchingCubes<TTarget,TPrimitive>::target_ptr p_target,
                                                                            const typename StructuredMarchingCubes<TTarget,TPrimitive>::Domain& r_domain,
                                                                            const typename StructuredMarchingCubes<TTarget,TPrimitive>::Resolution& r_numberOfPoints,
                                                                            typename StructuredMarchingCubes<TTarget,TPrimitive>::output_functor outputFunctor ) :
    StructuredMarchingPrimitives<TTarget,TPrimitive>(p_target,
                                                     r_domain,
                                                     r_numberOfPoints,
                                                     detail::getCubeEdgeMap<StructuredMarchingCubes::Dimension>(),
                                                     detail::getCubeConnectivityMap<StructuredMarchingCubes::Dimension>(),
                                                     outputFunctor)
{
}


template <concepts::Object TTarget, class TPrimitive>
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
inline void
StructuredMarchingCubes<TTarget,TPrimitive>::getVertices(Size primitiveIndex,
                                                         Ref<DynamicArray<Point>> r_vertexContainer) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    r_vertexContainer.clear();
    const auto numberOfPrimitives = std::ranges::views::transform(this->_numberOfPoints,
                                                                  [](Size numberOfPoints){return numberOfPoints - 1;});

    CartesianIndex primitiveCartesianIndex;
    this->getCartesianIndex(primitiveIndex,
                            numberOfPrimitives.begin(),
                            primitiveCartesianIndex.begin());

    // TODO

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
inline void
StructuredMarchingCubes<TTarget,TPrimitive>::getGlobalVertexIndex(Size primitiveIndex,
                                                                  Size vertexIndex,
                                                                  typename CartesianIndex::iterator it_indexBegin) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    const auto primitiveResolution = std::ranges::views::transform(this->_numberOfPoints,
                                                                   [](Size numberOfPoints){return numberOfPoints-1;});

    CartesianIndex primitiveCartesianIndex;
    this->getCartesianIndex(primitiveIndex,
                            primitiveResolution.begin(),
                            primitiveCartesianIndex.begin());

    auto it_primitiveIndex = primitiveCartesianIndex.begin();
    for (Size dim=0; dim<StructuredMarchingCubes::Dimension; ++dim, ++it_primitiveIndex, ++it_indexBegin)
        *it_indexBegin = utils::getBit(vertexIndex, dim) + (*it_primitiveIndex);

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
inline Size
StructuredMarchingCubes<TTarget,TPrimitive>::primitiveVertexCount() const
{
    return intPow(2, StructuredMarchingCubes<TTarget,TPrimitive>::Dimension);
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
inline Size
StructuredMarchingCubes<TTarget,TPrimitive>::numberOfRemainingPrimitives() const
{
    return std::accumulate(
        this->_numberOfPoints.begin(),
        this->_numberOfPoints.end(),
        1.0,
        []( Size lhs, Size rhs ) -> Size { return lhs * (rhs-1); }
    );
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
typename StructuredMarchingCubes<TTarget,TPrimitive>::Point
StructuredMarchingCubes<TTarget,TPrimitive>::getVertexOnPrimitive(const TPrimitive& r_primitive,
                                                                  Size vertexIndex) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    return detail::getVertexOnPrimitive(r_primitive, vertexIndex);

    CIE_END_EXCEPTION_TRACING
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
template <concepts::Cube T>
void
StructuredMarchingCubes<TTarget,TPrimitive>::checkMesh() const
{
    for ( auto meshEdgeLength : this->_meshEdgeLengths )
        CIE_CHECK(
            std::abs(meshEdgeLength - this->_meshEdgeLengths[0]) < 1e-15,
            "Invalid domain for StructuredMarchingCubes, consider using StructuredMarchingBoxes" )
}


template < concepts::Object TTarget,
           class TPrimitive >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
template <concepts::Box T>
void
StructuredMarchingCubes<TTarget,TPrimitive>::checkMesh() const
{
}


} // namespace cie::geo


#endif