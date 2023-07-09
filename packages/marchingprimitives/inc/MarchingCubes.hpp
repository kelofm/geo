#ifndef CIE_MESH_KERNEL_MARCHING_CUBES_HPP
#define CIE_MESH_KERNEL_MARCHING_CUBES_HPP

// --- GEO Includes ---
#include "packages/macros/inc/typedefs.hpp"
#include "packages/primitives/inc/Cube.hpp"
#include "packages/primitives/inc/Box.hpp"

// --- Mesh Includes ---
#include "packages/marchingprimitives/inc/MarchingPrimitives.hpp"

// --- STL Includes ---
#include <array>
#include <utility>
#include <functional>


namespace cie::geo {


namespace detail {
template <concepts::Cube TPrimitive>
typename TPrimitive::Point getVertexOnPrimitive(const TPrimitive& r_primitive,
                                                Size vertexIndex);

template <concepts::Box TPrimitive>
typename TPrimitive::Point getVertexOnPrimitive(const TPrimitive& r_primitive,
                                                Size vertexIndex);
} // namespace detail



template < concepts::Object TTarget,
           class TPrimitive = geo::Cube<TTarget::Dimension,typename TTarget::Coordinate> >
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
class UnstructuredMarchingCubes : public UnstructuredMarchingPrimitives<TTarget,TPrimitive>
{
public:
    using typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::Point;

    using typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::target_ptr;

    using typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::output_functor;

    using primitive_container     = std::vector<TPrimitive>;

    using primitive_container_ptr = std::shared_ptr<primitive_container>;

public:
    CIE_DEFINE_CLASS_DEFAULT_MOVES(UnstructuredMarchingCubes)

    UnstructuredMarchingCubes(target_ptr p_target,
                              primitive_container_ptr p_primitives,
                              output_functor outputFunctor);

    Point getVertexOnPrimitive(const TPrimitive& r_primitive,
                               Size vertexIndex) const override;

protected:
    virtual Size primitiveVertexCount() const override;
};




template <concepts::Object TTarget,
          class TPrimitive = geo::Cube<TTarget::Dimension,typename TTarget::Coordinate>>
requires (concepts::Cube<TPrimitive> || concepts::Box<TPrimitive>)
class StructuredMarchingCubes : public StructuredMarchingPrimitives<TTarget,TPrimitive>
{
private:
    using Base = StructuredMarchingPrimitives<TTarget,TPrimitive>;

public:
    using typename Base::Point;

    using typename Base::target_ptr;

    using typename Base::Domain;

    using typename Base::Resolution;

    using typename Base::output_functor;

    using typename Base::CartesianIndex;

public:
    StructuredMarchingCubes(target_ptr p_target,
                            const Domain& r_domain,
                            const Resolution& r_numberOfPoints,
                            output_functor outputFunctor);

    CIE_DEFINE_CLASS_DEFAULT_MOVES(StructuredMarchingCubes)

    virtual void getVertices(Size primitiveIndex,
                             Ref<DynamicArray<Point>> r_vertexContainer) const override;

    virtual void getGlobalVertexIndex(Size primitiveIndex,
                                      Size vertexIndex,
                                      typename CartesianIndex::iterator it_indexBegin) const override;

    virtual Size primitiveVertexCount() const override;

    virtual Size numberOfRemainingPrimitives() const override;

    Point getVertexOnPrimitive(const TPrimitive& r_primitive,
                               Size vertexIndex) const override;

private:
    /// Check whether the mesh domain can be discretized with cubes
    template <concepts::Cube T>
    void checkMesh() const;

    /// Do nothing
    template <concepts::Box T>
    void checkMesh() const;
};


} // namespace cie::geo

#include "packages/marchingprimitives/impl/MarchingCubes_impl.hpp"

#endif