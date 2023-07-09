#ifndef CIE_MESH_KERNEL_MARCHING_PRIMITIVES_HPP
#define CIE_MESH_KERNEL_MARCHING_PRIMITIVES_HPP

// --- Internal Includes ---
#include "packages/marchingprimitives/inc/AbsMarchingPrimitives.hpp"

namespace cie::geo {


template <concepts::Object TTarget, concepts::Primitive TPrimitive>
class MarchingPrimitives : public AbsMarchingPrimitives<TTarget>
{
public:
    using primitive_type = TPrimitive;

public:
    template <class ...Args>
    MarchingPrimitives(Args&&... args);

    virtual typename MarchingPrimitives<TTarget,TPrimitive>::Point getVertexOnPrimitive(const primitive_type& r_primitive, Size vertexIndex) const = 0;
};



/// MarchingPrimitives that scans a container of primitives
template <concepts::Object TTarget, concepts::Primitive TPrimitive>
class UnstructuredMarchingPrimitives : public MarchingPrimitives<TTarget,TPrimitive>
{
private:
    using Base = MarchingPrimitives<TTarget,TPrimitive>;

public:
    using Point = typename Base::Point;

    using primitive_container     = std::vector<typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::primitive_type>;

    using primitive_container_ptr = std::shared_ptr<primitive_container>;

public:
    UnstructuredMarchingPrimitives(typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::target_ptr p_target,
                                   const typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::edge_table& r_edgeTable,
                                   const typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::connectivity_table& r_connectivityTable,
                                   typename UnstructuredMarchingPrimitives<TTarget,TPrimitive>::output_functor outputFunctor,
                                   primitive_container_ptr p_primitives);

    virtual Point getVertex(Size primitiveIndex,
                            Size vertexIndex) override;

    virtual void getVertices(Size primitiveIndex,
                             Ref<DynamicArray<Point>> r_vertexContainer) const override;

    virtual Size numberOfRemainingPrimitives() const override;

    const primitive_container& primitives() const;

public:
    primitive_container_ptr _p_primitives;
};



/// MarchingPrimitives that scans a cartesian mesh of primitives
template <concepts::Object TTarget, concepts::Primitive TPrimitive>
class StructuredMarchingPrimitives : public MarchingPrimitives<TTarget,TPrimitive>
{
private:
    using Base = MarchingPrimitives<TTarget,TPrimitive>;

public:
    using typename Base::Point;

    using typename Base::Domain;

    using typename Base::Resolution;

    using typename Base::target_ptr;

    using typename Base::edge_table;

    using typename Base::connectivity_table;

    using typename Base::output_functor;

    using CartesianIndex = std::array<Size,MarchingPrimitives<TTarget,TPrimitive>::Dimension>;

public:
    StructuredMarchingPrimitives(target_ptr p_target,
                                 const Domain& r_domain,
                                 const Resolution& r_numberOfPoints,
                                 const edge_table& r_edgeTable,
                                 const connectivity_table& r_connectivityTable,
                                 output_functor outputFunctor);

    /// Return the index of the point in the cartesian mesh corresponding to the point of the specified primitive
    virtual void getGlobalVertexIndex(Size primitiveIndex,
                                      Size vertexIndex,
                                      typename CartesianIndex::iterator it_indexBegin) const = 0;

    virtual Point getVertex(Size primitiveIndex, Size vertexIndex) override;

protected:
    /// Return a point in the mesh specified by its cartesian index
    void getPointInCartesianMesh(typename CartesianIndex::const_iterator it_indexBegin,
                                 typename Point::iterator it_coordinateBegin) const;

    template <concepts::Iterator<Size> TIterator>
    static void getCartesianIndex(Size index,
                                  TIterator it_resolutionBegin,
                                  typename CartesianIndex::iterator it_indexBegin);

protected:
    Domain _domain;

    Resolution _numberOfPoints;

    Point _meshOrigin;

    Point _meshEdgeLengths;
};


} // namespace cie::geo

#include "packages/marchingprimitives/impl/MarchingPrimitives_impl.hpp"

#endif