#ifndef CIE_MESH_KERNEL_MARCHING_PRIMITIVES_IMPL_HPP
#define CIE_MESH_KERNEL_MARCHING_PRIMITIVES_IMPL_HPP

// --- Utility Includes ---
#include "packages/stl_extension/inc/resize.hpp"
#include "packages/maths/inc/bit.hpp"
#include "packages/maths/inc/power.hpp"
#include "packages/macros/inc/exceptions.hpp"
#include "packages/macros/inc/checks.hpp"

// --- STL Includes ---
#include <algorithm>
#include <numeric>


namespace cie::geo {


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
template <class ...Args>
MarchingPrimitives<TTarget,TPrimitive>::MarchingPrimitives(Args&&... args) :
    AbsMarchingPrimitives<TTarget>(std::forward<Args>(args)...)
{
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
UnstructuredMarchingPrimitives<TTarget,TPrimitive>::UnstructuredMarchingPrimitives(typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::target_ptr p_target,
                                                                                          const typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::edge_table& r_edgeTable,
                                                                                          const typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::connectivity_table& r_connectivityTable,
                                                                                          typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::output_functor outputFunctor,
                                                                                          typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::primitive_container_ptr p_primitives) :
    MarchingPrimitives<TTarget,TPrimitive>(p_target,
                                                 r_edgeTable,
                                                 r_connectivityTable,
                                                 outputFunctor),
    _p_primitives(p_primitives)
{
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
inline typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::Point
UnstructuredMarchingPrimitives<TTarget,TPrimitive>::getVertex(Size primitiveIndex,
                                                              Size vertexIndex)
{
    return this->getVertexOnPrimitive(this->_p_primitives->at(primitiveIndex),
                                      vertexIndex);
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
inline void
UnstructuredMarchingPrimitives<TTarget,TPrimitive>::getVertices(Size primitiveIndex,
                                                                Ref<DynamicArray<Point>> r_vertexContainer) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    r_vertexContainer.clear();
    const auto& r_primitive = this->_p_primitives->at(primitiveIndex);
    for (Size i_vertex=0; i_vertex<this->primitiveVertexCount(); ++i_vertex)
        r_vertexContainer.push_back(this->getVertexOnPrimitive(r_primitive, i_vertex));

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
inline Size
UnstructuredMarchingPrimitives<TTarget,TPrimitive>::numberOfRemainingPrimitives() const
{
    return this->_p_primitives->size();
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
inline const typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::primitive_container&
UnstructuredMarchingPrimitives<TTarget,TPrimitive>::primitives() const
{
    return *this->_p_primitives;
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
StructuredMarchingPrimitives<TTarget,TPrimitive>::StructuredMarchingPrimitives(target_ptr p_target,
                                                                               const Domain& r_domain,
                                                                               const Resolution& r_numberOfPoints,
                                                                               const edge_table& r_edgeTable,
                                                                               const connectivity_table& r_connectivityTable,
                                                                               output_functor outputFunctor)
    : MarchingPrimitives<TTarget,TPrimitive>(p_target,
                                             r_edgeTable,
                                             r_connectivityTable,
                                             outputFunctor),
      _domain(r_domain),
      _numberOfPoints(r_numberOfPoints),
      _meshOrigin(),
      _meshEdgeLengths()
{
    CIE_BEGIN_EXCEPTION_TRACING

    utils::resize(this->_meshOrigin, StructuredMarchingPrimitives::Dimension);
    utils::resize(this->_meshEdgeLengths, StructuredMarchingPrimitives::Dimension);

    for (Size dim=0; dim<StructuredMarchingPrimitives::Dimension; ++dim)
    {
        this->_meshOrigin[dim] = r_domain[dim].first;

        CIE_DIVISION_BY_ZERO_CHECK(r_numberOfPoints[dim] > 1)

        this->_meshEdgeLengths[dim] = (r_domain[dim].second - r_domain[dim].first) / (r_numberOfPoints[dim] - 1);

        CIE_DEBUG_CHECK(
            this->_meshEdgeLengths[dim] > 0,
            "Invalid domain"
        )
    }

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
inline typename StructuredMarchingPrimitives<TTarget,TPrimitive>::Point
StructuredMarchingPrimitives<TTarget,TPrimitive>::getVertex(Size primitiveIndex,
                                                            Size vertexIndex)
{
    CIE_BEGIN_EXCEPTION_TRACING

    CartesianIndex cartesianIndex;
    this->getGlobalVertexIndex(primitiveIndex, vertexIndex, cartesianIndex.begin());

    Point output;
    this->getPointInCartesianMesh(cartesianIndex.begin(), output.begin());

    return output;

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
inline void
StructuredMarchingPrimitives<TTarget,TPrimitive>::getPointInCartesianMesh(typename CartesianIndex::const_iterator it_indexBegin,
                                                                          typename Point::iterator it_coordinateBegin) const
{
    CIE_BEGIN_EXCEPTION_TRACING

    auto it_origin = _meshOrigin.begin();
    auto it_length = _meshEdgeLengths.begin();
    const auto it_end = it_coordinateBegin + StructuredMarchingPrimitives<TTarget,TPrimitive>::Dimension;

    for (; it_coordinateBegin!=it_end; ++it_coordinateBegin, ++it_indexBegin, ++it_origin, ++it_length)
        *it_coordinateBegin = (*it_origin) + (*it_indexBegin) * (*it_length);

    CIE_END_EXCEPTION_TRACING
}


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
template <concepts::Iterator<Size> TIterator>
inline void
StructuredMarchingPrimitives<TTarget,TPrimitive>::getCartesianIndex(Size index,
                                                                    TIterator it_resolutionBegin,
                                                                    typename CartesianIndex::iterator it_indexBegin)
{
    CIE_BEGIN_EXCEPTION_TRACING

    CartesianIndex subgridPrefixes;
    std::exclusive_scan(it_resolutionBegin,
                        it_resolutionBegin + StructuredMarchingPrimitives::Dimension,
                        subgridPrefixes.begin(),
                        1,
                        [](Size lhs, Size rhs){return lhs*rhs;});

    auto it_prefix = subgridPrefixes.end() - 1;
    const auto it_end = it_indexBegin + StructuredMarchingPrimitives::Dimension;
    for (; it_indexBegin!=it_end; ++it_indexBegin, --it_prefix)
    {
        *it_indexBegin = index / (*it_prefix);
        index -= (*it_indexBegin) * (*it_prefix);
    }

    CIE_OUT_OF_RANGE_CHECK(index == 0)

    CIE_END_EXCEPTION_TRACING
}


} // namespace cie::geo


#endif